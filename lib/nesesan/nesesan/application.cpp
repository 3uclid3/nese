#include <nesesan/application.hpp>

#include <imgui.h>

namespace nese::san {

void application::update(f32_t dt)
{
    update_main_menu();

    const ImGuiIO& io = ImGui::GetIO();

    // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
    if (_show_imgui_demo)
    {
        ImGui::ShowDemoWindow(&_show_imgui_demo);
    }

    // 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
    {
        static float f = 0.0f;
        static int counter = 0;

        ImGui::Begin("Hello, world!"); // Create a window called "Hello, world!" and append into it.

        ImGui::Text("This is some useful text.");          // Display some text (you can use a format strings too)
        ImGui::Checkbox("Demo Window", &_show_imgui_demo); // Edit bools storing our window open/close state
        ImGui::Checkbox("Another Window", &_show_another_window);

        ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f

        if (ImGui::Button("Button")) // Buttons return true when clicked (most widgets return true when edited/activated)
            counter++;
        ImGui::SameLine();
        ImGui::Text("counter = %d", counter);

        ImGui::Text("Application average %.3f ms/frame (%.1f FPS) (%.2f DT)", 1000.0f / io.Framerate, io.Framerate, dt);
        ImGui::End();
    }

    // 3. Show another simple window.
    if (_show_another_window)
    {
        ImGui::Begin("Another Window", &_show_another_window); // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
        ImGui::Text("Hello from another window!");
        if (ImGui::Button("Close Me"))
            _show_another_window = false;
        ImGui::End();
    }
}

void application::exit()
{
    _exit_requested = true;
}


void application::update_main_menu()
{
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("Exit"))
            {
                exit();
            }

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Debug"))
        {

            ImGui::EndMenu();
        }


        ImGui::EndMainMenuBar();
    }
}

} // namespace nesesan
