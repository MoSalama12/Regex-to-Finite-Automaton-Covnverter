#include <iostream>

#include "regularExpression.h"
#include "NFA.h"

#include<vector>
#include<set>
#include<cstring>

#include "Walnut/Application.h"
#include "Walnut/EntryPoint.h"

#include "Walnut/Image.h"

const static float CIRCLE_RADIUS = 32.0 * 2;

const int MAX_STATE_COUNT = 1000;

class Diagram
{
public:
    ImVec2 pos;
    float radius;
    std::string state;
    std::vector<std::pair<Diagram*, std::string>> next;

    bool isStart, isAccept;
public:

    Diagram(ImVec2 pos, std::string state)
    {
        this->pos = pos;
        this->radius = CIRCLE_RADIUS;
        this->next = {};
        this->state = state;
        this->isAccept = false;
        this->isStart = false;
    }

    bool isCollisionOccured(ImVec2 cursor)
    {
        return ((cursor.x - pos.x) * (cursor.x - pos.x) +
            (cursor.y - pos.y) * (cursor.y - pos.y) <= radius * radius);
    }

    bool operator == (Diagram & obj) 
    {
        return (this->state == obj.state);
    }

    Diagram* getNextFromSymbole(const char & symbole)
    {
        for (size_t i = 0; i < next.size(); i++)
        {
            if (next[i].second[0] == symbole)
            {
                return next[i].first;
            }
        }
        return NULL;
    }
};

void dfs(int src, bool* vis, std::vector<int>** nfaTable, int indx);
bool isAcceptedText( const std::vector<Diagram*> points, const int& startIndex, const std::string& Text);

class ExampleLayer : public Walnut::Layer
{
public:
	virtual void OnUIRender() override
	{


        bool openWindow = true;
		ImGui::Begin("Main" , &openWindow, ImGuiWindowFlags_NoTitleBar);

        static char regualrExpression[128] = "a.(a|b)";
        static char stringValidityTest[128] = "";

        static int algorithmNum = 0;
        static bool opt_enable_grid = true;
        static bool opt_enable_context_menu = false;
        static ImVec4 colf1 = ImVec4(1.0f, 1.0f, 0.4f, 1.0f);
        static ImVec4 colf2 = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);

        static std::vector<Diagram*> points;

        static int startIndex = -1;
        
        

        {
            ImGuiWindowFlags window_flags = ImGuiWindowFlags_HorizontalScrollbar;
            ImGui::BeginChild("ChildL", ImVec2(ImGui::GetContentRegionAvail().x * 0.25f, ImGui::GetContentRegionAvail().y), false, window_flags);
            

            ImGui::InputTextWithHint("Enter Expression", "Regular Expression", regualrExpression, IM_ARRAYSIZE(regualrExpression));
            
            if (ImGui::Button("Submit"))
            {
                std::string expression = regualrExpression;
                regularExpression x(expression);
                std::set<char> symbols = x.getSymbols();

                std::queue<std::string> output = x.parseString();
                NFA nfa;
                nfa.constructNFA(output);

                std::vector < std::vector < std::string>> data = nfa.constructTransitionTable();

                // Reset
                points.clear();
                startIndex = -1;

                if (algorithmNum == 0) // NFA
                {
                    
                    std::set<std::string> collectStates;
                    for (size_t i = 0; i < data.size(); i++)
                    {
                        collectStates.insert(data[i][0]);
                        collectStates.insert(data[i][1]);
                    }

                    int i = 0;
                    for (auto x : collectStates)
                    {
                        points.push_back(new Diagram(ImVec2(150 + (i * 100), 100), x));
                        i++;
                    }


                    for (size_t i = 0; i < data.size(); i++)
                    {
                        points[std::stoi(data[i][0]) - 1]->next.push_back(std::make_pair(points[std::stoi(data[i][1]) - 1],data[i][2]));
                    }


                    std::set<std::string> collectStatesFromNextOnly;
                    for (size_t i = 0; i < points.size(); i++)
                    {
                        if (points[i]->next.size() == 0)
                        {
                            points[i]->isAccept = true;
                        }

                        for (size_t j = 0; j < points[i]->next.size(); j++)
                        {
                            collectStatesFromNextOnly.insert(points[i]->next[j].first->state);
                        }
                    }

                    for (std::string x : collectStates)
                    {
                        if (collectStatesFromNextOnly.find(x) == collectStatesFromNextOnly.end())
                        {
                            points[std::stoi(x) - 1]->isStart = true;
                            break;
                        }
                    }
                }
                else { // DFA
		    nfa.setStartState();
                    State startState = nfa.getStartState();
                    int stateCount = startState.getStatesCount();
                    int symbolsCount = symbols.size() + 1;
                    int start = startState.getNumber();

                    std::vector<int>** nfaTable = nfa.constructTransitionTable2();
                    std::set<int> epClosure;
                    std::vector<std::set<int>> allTransitions;
                    std::map<std::pair<std::set<int>, int>, std::set<int>> dfaTable;

                    // DFA Code

                    bool vis[MAX_STATE_COUNT + 2];
                    memset(vis, 0, sizeof vis);
                    dfs(start, vis, nfaTable, 0);

                    for (int i = 0; i <= stateCount; i++)
                    {
                        if (vis[i]) epClosure.insert(i);
                    }

                    allTransitions.push_back(epClosure);
                    for (int currentTransition = 0; currentTransition < allTransitions.size(); currentTransition++)
                    {
                        std::set<int> from = allTransitions[currentTransition];
                        for (int i = 1; i < symbolsCount; i++)
                        {
                            bool visited[MAX_STATE_COUNT + 2] = { 0 }, v[MAX_STATE_COUNT + 2] = { 0 };

                            if (std::find(from.begin(), from.end(), start) != from.end())
                            {
                                v[start] = 1;
                            }
                            for (auto f : from)
                            {
                                for (auto node : nfaTable[f][i])
                                {
                                    if (!visited[node])
                                    {
                                        dfs(node, visited, nfaTable, i);
                                        dfs(node, visited, nfaTable, 0);
                                    }
                                }
                            }

                            std::set<int> to;
                            for (int j = 0; j <= stateCount; j++)
                            {
                                if (visited[j])
                                {
                                    to.insert(j);
                                }
                            }

                            dfaTable[{from, i}] = to;
                            if (find(allTransitions.begin(), allTransitions.end(), to) == allTransitions.end())
                            {
                                allTransitions.push_back(to);
                            }
                        }

                    }
                    

                }
            }
            ImGui::Separator();

            if (algorithmNum == 1 && startIndex != -1)
            {
                ImGui::InputTextWithHint("Enter Text", "Validity Test", stringValidityTest, IM_ARRAYSIZE(stringValidityTest));

                bool isAccepted = isAcceptedText(points, startIndex, stringValidityTest);
                if (isAccepted)
                    ImGui::TextColored(ImColor(0, 200, 0, 255), "Accepted");
                else
                    ImGui::TextColored(ImColor(200, 0, 0, 255), "Rejected");

                ImGui::Separator();
            }


            ImGui::ColorEdit4("Color 1", &colf1.x);
            ImGui::ColorEdit4("Color 2", &colf2.x);

            ImGui::RadioButton("NFA", &algorithmNum, 0); ImGui::SameLine();
            ImGui::RadioButton("DFA", &algorithmNum, 1);

            ImGui::Checkbox("Enable grid", &opt_enable_grid);
            //ImGui::Checkbox("Enable context menu", &opt_enable_context_menu);
            ImGui::Text("Mouse Left: drag to add lines,\nMouse Right: drag to scroll, click for context menu.");

            ImGui::EndChild();
        }

        ImGui::SameLine();

        // Child 2: rounded border
        {
            ImGuiWindowFlags window_flags = ImGuiWindowFlags_None;

            ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);
            ImGui::BeginChild("ChildR", ImVec2(0, ImGui::GetContentRegionAvail().y), true, window_flags);

            static ImVec2 scrolling(0.0f, 0.0f);
            static int editing_Circle = -1;
            
            ImVec2 canvas_p0 = ImGui::GetCursorScreenPos();      // ImDrawList API uses screen coordinates!
            ImVec2 canvas_sz = ImGui::GetContentRegionAvail();   // Resize canvas to what's available
            if (canvas_sz.x < 50.0f) canvas_sz.x = 50.0f;
            if (canvas_sz.y < 50.0f) canvas_sz.y = 50.0f;
            ImVec2 canvas_p1 = ImVec2(canvas_p0.x + canvas_sz.x, canvas_p0.y + canvas_sz.y);

            // Draw border and background color
            ImGuiIO& io = ImGui::GetIO();
            ImDrawList* draw_list = ImGui::GetWindowDrawList();
            draw_list->AddRectFilled(canvas_p0, canvas_p1, IM_COL32(50, 50, 50, 255));
            draw_list->AddRect(canvas_p0, canvas_p1, IM_COL32(255, 255, 255, 255));

            // This will catch our interactions
            ImGui::InvisibleButton("canvas", canvas_sz, ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonRight);
            const bool is_hovered = ImGui::IsItemHovered(); // Hovered
            const bool is_active = ImGui::IsItemActive();   // Held
            const ImVec2 origin(canvas_p0.x + scrolling.x, canvas_p0.y + scrolling.y); // Lock scrolled origin
            const ImVec2 mouse_pos_in_canvas(io.MousePos.x - origin.x, io.MousePos.y - origin.y);

            // Add first and second point
            if (is_hovered && editing_Circle == -1 && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
            {
                for (int i = 0; i < points.size(); i++)
                {
                    if (points[i]->isCollisionOccured(mouse_pos_in_canvas))
                    {
                        editing_Circle = i;
                    }
                }
                /*if (editing_Circle == -1)
                {
                    points.push_back(new Diagram(mouse_pos_in_canvas , "S0"));
                }
                else {
                    std::cout << "Circle found in this place ." << std::endl;
                }*/
            }
            if (editing_Circle != -1)
            {
                points[editing_Circle]->pos = mouse_pos_in_canvas;
                if (!ImGui::IsMouseDown(ImGuiMouseButton_Left))
                    editing_Circle = -1;
            }

            // Pan (we use a zero mouse threshold when there's no context menu)
            // You may decide to make that threshold dynamic based on whether the mouse is hovering something etc.
            const float mouse_threshold_for_pan = opt_enable_context_menu ? -1.0f : 0.0f;
            if (is_active && ImGui::IsMouseDragging(ImGuiMouseButton_Right, mouse_threshold_for_pan))
            {
                scrolling.x += io.MouseDelta.x;
                scrolling.y += io.MouseDelta.y;
            }

            // Context menu (under default mouse threshold)
            ImVec2 drag_delta = ImGui::GetMouseDragDelta(ImGuiMouseButton_Right);
            if (opt_enable_context_menu && drag_delta.x == 0.0f && drag_delta.y == 0.0f)
                ImGui::OpenPopupOnItemClick("context", ImGuiPopupFlags_MouseButtonRight);
            if (ImGui::BeginPopup("context"))
            {
                if (editing_Circle != -1)
                    points.resize(points.size() - 1);
                editing_Circle = -1;
                //if (ImGui::MenuItem("Remove one", NULL, false, points.size() > 0)) { points.resize(points.size() - 1); }
                if (ImGui::MenuItem("Remove all", NULL, false, points.size() > 0)) { points.clear(); }
                ImGui::EndPopup();
            }

            // Draw grid + all lines in the canvas
            draw_list->PushClipRect(canvas_p0, canvas_p1, true);
            if (opt_enable_grid)
            {
                const float GRID_STEP = 64.0f;
                for (float x = fmodf(scrolling.x, GRID_STEP); x < canvas_sz.x; x += GRID_STEP)
                    draw_list->AddLine(ImVec2(canvas_p0.x + x, canvas_p0.y), ImVec2(canvas_p0.x + x, canvas_p1.y), IM_COL32(200, 200, 200, 40));
                for (float y = fmodf(scrolling.y, GRID_STEP); y < canvas_sz.y; y += GRID_STEP)
                    draw_list->AddLine(ImVec2(canvas_p0.x, canvas_p0.y + y), ImVec2(canvas_p1.x, canvas_p0.y + y), IM_COL32(200, 200, 200, 40));
            }
            for (int n = 0; n < points.size(); n++)
            {
                draw_list->AddCircle(ImVec2(origin.x + points[n]->pos.x, origin.y + points[n]->pos.y), CIRCLE_RADIUS, ImColor(colf1), 0.0f, 2.0f);

                if (points[n]->isAccept)
                    draw_list->AddCircle(ImVec2(origin.x + points[n]->pos.x, origin.y + points[n]->pos.y), CIRCLE_RADIUS - 6, ImColor(colf1), 0.0f, 2.0f);

                if (points[n]->isStart)
                    draw_list->AddText(ImVec2(origin.x + points[n]->pos.x - 17, origin.y + points[n]->pos.y + CIRCLE_RADIUS), ImColor(colf2), "Start");

                std::string s("S" + std::to_string(n + 1));
                const char * char_array = s.c_str();
                draw_list->AddText(ImVec2(origin.x + points[n]->pos.x - 6, origin.y + points[n]->pos.y -6), ImColor(colf2), char_array);

                std::vector< std::pair<ImVec2 , ImVec2>> collectLines;
                for (int i = 0; i < points[n]->next.size(); i++)
                {
                    if (points[n] == points[n]->next[i].first)
                    {
                        ImVec2 cp4[4] = { ImVec2(origin.x + points[n]->pos.x + CIRCLE_RADIUS, origin.y + points[n]->pos.y), 
                            ImVec2(origin.x + points[n]->pos.x + CIRCLE_RADIUS, origin.y + points[n]->pos.y + 125),
                            ImVec2(origin.x + points[n]->next[i].first->pos.x - CIRCLE_RADIUS, origin.y + points[n]->next[i].first->pos.y + 125),
                            ImVec2(origin.x + points[n]->next[i].first->pos.x - CIRCLE_RADIUS, origin.y + points[n]->next[i].first->pos.y) };
                        draw_list->AddBezierCubic(cp4[0], cp4[1] , cp4[2], cp4[3], ImColor(colf2), 2.0f);

                        ImVec2 cp3[3] = { ImVec2(origin.x + points[n]->next[i].first->pos.x + CIRCLE_RADIUS, origin.y + points[n]->next[i].first->pos.y),
                            ImVec2(origin.x + points[n]->next[i].first->pos.x + CIRCLE_RADIUS - 10, origin.y + points[n]->next[i].first->pos.y + 10),
                            ImVec2(origin.x + points[n]->next[i].first->pos.x + CIRCLE_RADIUS + 10, origin.y + points[n]->next[i].first->pos.y + 10)
                        };
                        draw_list->AddTriangleFilled(cp3[0], cp3[1], cp3[2], ImColor(colf2));

                        char* char_array2 = &points[n]->next[i].second[0];

                        draw_list->AddText(ImVec2(origin.x + ((points[n]->next[i].first->pos.x + points[n]->pos.x) / 2), origin.y + ((points[n]->next[i].first->pos.y + points[n]->pos.y) / 2) + 100), ImColor(colf2), char_array2);
                    }
                    else {

                        bool findLineDuplicates = false;
                        ImVec2 startPoint = ImVec2(origin.x + points[n]->pos.x + CIRCLE_RADIUS, origin.y + points[n]->pos.y);
                        ImVec2 endPoint = ImVec2(origin.x + points[n]->next[i].first->pos.x - CIRCLE_RADIUS - 10, origin.y + points[n]->next[i].first->pos.y);
                        for (int i = 0; i < collectLines.size(); i++)
                        {
                            if (collectLines[i].first.x == startPoint.x && collectLines[i].first.y == startPoint.y &&
                                collectLines[i].second.x == endPoint.x && collectLines[i].second.y == endPoint.y)
                            {
                                findLineDuplicates = true;
                                break;
                            }
                        }

                        draw_list->AddLine(startPoint,endPoint, ImColor(colf2), 2.0f);

                        collectLines.push_back(std::make_pair(startPoint , endPoint));

                        ImVec2 cp3[3] = {ImVec2(origin.x + points[n]->next[i].first->pos.x - CIRCLE_RADIUS, origin.y + points[n]->next[i].first->pos.y),
                            ImVec2(origin.x + points[n]->next[i].first->pos.x - CIRCLE_RADIUS - 10, origin.y + points[n]->next[i].first->pos.y - 10),
                            ImVec2(origin.x + points[n]->next[i].first->pos.x - CIRCLE_RADIUS - 10, origin.y + points[n]->next[i].first->pos.y + 10) 
                        };
                        draw_list->AddTriangleFilled(cp3[0], cp3[1], cp3[2], ImColor(colf2));

                        char* char_array2 = &points[n]->next[i].second[0];

                        if (findLineDuplicates)
                            draw_list->AddText(ImVec2(origin.x + ((points[n]->next[i].first->pos.x + points[n]->pos.x) / 2) + 10, 
                                origin.y + (points[n]->next[i].first->pos.y + points[n]->pos.y) / 2), ImColor(colf2), char_array2);
                        else
                            draw_list->AddText(ImVec2(origin.x + ((points[n]->next[i].first->pos.x + points[n]->pos.x) / 2),
                                origin.y + (points[n]->next[i].first->pos.y + points[n]->pos.y) / 2), ImColor(colf2), char_array2);
                    }
                }
            }
                
            draw_list->PopClipRect();



            ImGui::EndChild();
            ImGui::PopStyleVar();
        }


		ImGui::End();

		//ImGui::ShowDemoWindow();
	}
};

Walnut::Application* Walnut::CreateApplication(int argc, char** argv)
{
	Walnut::ApplicationSpecification spec;
	spec.Name = "Theory of computing";

	Walnut::Application* app = new Walnut::Application(spec);
	app->PushLayer<ExampleLayer>();

	/*app->SetMenubarCallback([app]()
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Exit"))
			{
				app->Close();
			}
			ImGui::EndMenu();
		}
	});*/
	return app;
}

bool isAcceptedText(const std::vector<Diagram*> points, const int& startIndex, const std::string& Text)
{
    Diagram* currentPoint = points[startIndex];
    for (size_t i = 0; i < Text.size(); i++)
    {
        currentPoint = currentPoint->getNextFromSymbole(Text[i]);

        if (currentPoint == NULL)
            return false;
    }

    return currentPoint->isAccept;
}
