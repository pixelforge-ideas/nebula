//------------------------------------------------------------------------------
//  outline.cc
//  (C) 2021 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "foundation/stdneb.h"
#include "outline.h"
#include "editor/editor.h"
#include "editor/commandmanager.h"
#include "editor/ui/uimanager.h"
#include "editor/tools/selectioncontext.h"
#include "editor/cmds.h"
#include "core/cvar.h"

using namespace Editor;

namespace Presentation
{
__ImplementClass(Presentation::Outline, 'OtWn', Presentation::BaseWindow);

//------------------------------------------------------------------------------
/**
*/
Outline::Outline()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
Outline::~Outline()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
Outline::Run(SaveMode save)
{
    static bool renaming = false;
    const char* filterPopup = "outline_filter_popup";
    static bool filtering = false;
    static ImGuiTextFilter nameFilter;
    static ImGuiTextFilter guidFilter;
    static ImGuiTextFilter tagFilter;
    static ImGuiTextFilter blueprintFilter;
    static float filterDistance = 0.0f;

    static Core::CVar* cl_debug_worlds = Core::CVarGet("cl_debug_worlds");

    if (ImGui::Button("Filter"))
    {
        ImGui::OpenPopup(filterPopup);
    }
    ImGui::SameLine();
    nameFilter.Draw("Search", 180);
    if (filtering) 
    {
        ImGui::SameLine();
        if (ImGui::Button("Reset"))
        {
            nameFilter.Clear();
            guidFilter.Clear();
            tagFilter.Clear();
            blueprintFilter.Clear();
            filterDistance = 0.0f;
        }
    }

    // always reset filtering status
    filtering = false;
    // set filtering to true if any filter is active
    filtering |= nameFilter.IsActive();
    filtering |= tagFilter.IsActive();
    filtering |= blueprintFilter.IsActive();
    filtering |= filterDistance != 0.0f;
    
    ImVec2 windowPos = ImGui::GetWindowPos();
    float windowWidth = ImGui::GetWindowWidth();
    if (ImGui::BeginPopup(filterPopup))
    {
        ImGui::SetWindowPos({windowPos.x + windowWidth, windowPos.y});
        ImGui::Text("Filter outline");
        ImGui::Separator();
        nameFilter.Draw("Name", 180);
        tagFilter.Draw("Tag", 180);
        blueprintFilter.Draw("Blueprint", 180);

        ImGui::Separator();
        ImGui::SliderFloat("Distance", &filterDistance, 0.0f, 1000.0f);
        ImGui::SameLine();
        ImGui::TextDisabled(" -Disabled");
        // TODO: Filter by property, spatially, etc.
        ImGui::EndPopup();
    }

    ImGui::Separator();

    // @todo:   The order of the entites is arbitrary. We should allow the user to move and sort the entities as they like.
    ImGui::BeginChild("ScrollingRegion", ImVec2(0, -ImGui::GetFrameHeightWithSpacing()), false, ImGuiWindowFlags_HorizontalScrollbar);
    {
        Game::Filter filter = Game::FilterBuilder().Including<Game::Entity>().Build();
        Game::Dataset data = state.editorWorld->Query(filter);
        
        bool contextMenuOpened = false;
        ImGui::NewLine();
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, {0, 0});
        ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, {0, 0});
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, {4, 0});

        for (int v = 0; v < data.numViews; v++)
        {
            Game::Dataset::View const& view = data.views[v];
            Editor::Entity const* const entities = (Editor::Entity*)view.buffers[0];
            
            for (IndexT i = 0; i < view.numInstances; ++i)
            {
                Editor::Entity const& entity = entities[i];
                Editable& edit = state.editables[entity.index];

                if (!state.editorWorld->IsValid(entity))
                    continue;

                if (!nameFilter.PassFilter(edit.name.AsCharPtr()))
                    continue;
                
                ImGui::BeginGroup();
                {
                    bool selected = Tools::SelectionContext::Selection().BinarySearchIndex(entity) != InvalidIndex;
                    ImGui::BeginGroup();
                    // FIXME: rightclicking a selectable when another is selected does not bring up the context menu for the correct entity
                    ImGui::PushID(entity.HashCode());
                    float const xPos = ImGui::GetCursorPosX();
                    float const yPos = ImGui::GetCursorPosY();
                    // TODO: We should probably switch to TreeNodes. See here: https://github.com/ocornut/imgui/issues/1861 
                    if (ImGui::Selectable(
                        "##entityselect",
                        selected,
                        //ImGuiSelectableFlags_::ImGuiSelectableFlags_AllowDoubleClick |
                        ImGuiSelectableFlags_::ImGuiSelectableFlags_SpanAllColumns |
                        ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups,
                        {ImGui::GetColumnWidth(), 20}
                    ))
                    {
                        Util::Array<Editor::Entity> selection;
                        if (ImGui::GetIO().KeyCtrl)
                        {
                            selection = Tools::SelectionContext::Selection();
                            if (selected)
                            {
                                if (!ImGui::IsItemClicked(1))
                                {
                                    IndexT index = selection.BinarySearchIndex(entity);
                                    selection.EraseIndex(index);
                                    selected = false;
                                }
                            }
                            else
                            {
                                selection.InsertSorted(entity);
                                selected = true;
                            }
                        }
                        else if (!selected || (Tools::SelectionContext::Selection().Size() > 1 && !ImGui::IsItemClicked(1)))
                        {
                            selection.InsertSorted(entity);
                            selected = true;
                        }
                        if (!selection.IsEmpty())
                        {
                            Edit::SetSelection(selection);
                        }
                    }
                    if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
                    {
                        static Game::Entity entityPayload;
                        entityPayload = entity;
                        ImGui::SetDragDropPayload("entity", &entityPayload, sizeof(Game::Entity));
                        ImGui::EndDragDropSource();
                    }
                    
                    ImGui::SameLine();
                    ImGui::SetCursorPosX(xPos);
                    ImGui::AlignTextToFramePadding();
                    ImGui::TextDisabled("|-");
                    ImGui::SameLine();
                    
                    // TODO: make this work again
                    //ImGui::Image(&UIManager::Icons::game, {20,20});
                    ImGui::Text(" "); // hack: this just replaces the image until that works again

                    ImGui::SameLine();
                    ImGui::AlignTextToFramePadding();
                    ImGui::Text(edit.name.AsCharPtr());
                    
                    if (Core::CVarReadInt(cl_debug_worlds) > 0)
                    {
                        ImGui::SameLine();
                        ImGuiStyle const& style = ImGui::GetStyle();
                        float widthNeeded = ImGui::CalcTextSize(edit.guid.AsString().AsCharPtr()).x + style.FramePadding.x * 2.f + style.ItemSpacing.x;
                        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + Math::max(ImGui::GetContentRegionAvail().x - widthNeeded, 0.0f));
                        ImGui::AlignTextToFramePadding();
                        ImGui::TextColored({0.1,0.2f,0.1f,0.5f}, edit.guid.AsString().AsCharPtr());
                    }
                    ImGui::PopID();
                    ImGui::EndGroup();
                    
                    if (!contextMenuOpened && selected && ImGui::BeginPopupContextWindow())
                    {
                        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, {4, 5});
                        contextMenuOpened = true;
                        if (ImGui::Selectable("Copy"))
                        {
                            // TODO: implement
                        }
                        if (ImGui::Selectable("Paste"))
                        {
                            // TODO: implement
                        }
                        ImGui::Separator();
                        if (ImGui::Selectable("Duplicate"))
                        {
                            // TODO: implement
                            // Editor::SceneTree::Instance()->DuplicateEntity(hNode.entity);
                        }
                        if (ImGui::Selectable("New child"))
                        {
                            // TODO: Implement
                        }
                        ImGui::Separator();
                        if (ImGui::Selectable("Delete"))
                        {
                            auto selection = Tools::SelectionContext::Selection();
                            Edit::CommandManager::BeginMacro("Delete entities", false);
                            Util::Array<Editor::Entity> emptySelection;
                            Edit::SetSelection(emptySelection);
                            for (auto e : selection)
                            {
                                Edit::DeleteEntity(e);
                            }
                            Edit::CommandManager::EndMacro();
                            
                        }
                        ImGui::PopStyleVar();
                        ImGui::EndPopup();
                    }
                }
                ImGui::EndGroup();
            }
        }

        ImGui::PopStyleVar(3);

        Game::DestroyFilter(filter);
    }
    ImGui::EndChild();
}

} // namespace Presentation
