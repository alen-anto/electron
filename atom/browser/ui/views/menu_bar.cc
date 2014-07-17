// Copyright (c) 2014 GitHub, Inc. All rights reserved.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

#include "atom/browser/ui/views/menu_bar.h"

#include "atom/browser/ui/views/menu_delegate.h"
#include "ui/base/models/menu_model.h"
#include "ui/views/background.h"
#include "ui/views/controls/button/menu_button.h"
#include "ui/views/layout/box_layout.h"

namespace atom {

namespace {

const char kViewClassName[] = "AtomMenuBar";

// Default color of the menu bar.
const SkColor kDefaultColor = SkColorSetARGB(255, 233, 233, 233);

}  // namespace

MenuBar::MenuBar()
    : menu_model_(NULL) {
  set_background(views::Background::CreateSolidBackground(kDefaultColor));
  SetLayoutManager(new views::BoxLayout(
      views::BoxLayout::kHorizontal, 0, 0, 0));
}

MenuBar::~MenuBar() {
}

void MenuBar::SetMenu(ui::MenuModel* model) {
  menu_model_ = model;
  RemoveAllChildViews(true);

  for (int i = 0; i < model->GetItemCount(); ++i) {
    views::MenuButton* button =
        new views::MenuButton(this, model->GetLabelAt(i), this, false);
    button->set_tag(i);
    AddChildView(button);
  }
}

int MenuBar::GetItemCount() const {
  return menu_model_->GetItemCount();
}

bool MenuBar::GetMenuButtonFromScreenPoint(const gfx::Point& point,
                                           ui::MenuModel** menu_model,
                                           views::MenuButton** button) {
  gfx::Point location(point);
  views::View::ConvertPointFromScreen(this, &location);

  if (location.x() < 0 || location.x() >= width() || location.y() < 0 ||
      location.y() >= height())
    return false;

  for (int i = 0; i < child_count(); ++i) {
    views::View* view = child_at(i);
    if (view->bounds().Contains(location) &&
        (menu_model_->GetTypeAt(i) == ui::MenuModel::TYPE_SUBMENU)) {
      *menu_model = menu_model_->GetSubmenuModelAt(i);
      *button = static_cast<views::MenuButton*>(view);
      return true;
    }
  }

  return false;
}

const char* MenuBar::GetClassName() const {
  return kViewClassName;
}

void MenuBar::ButtonPressed(views::Button* sender, const ui::Event& event) {
}

void MenuBar::OnMenuButtonClicked(views::View* source,
                                  const gfx::Point& point) {
  if (!menu_model_)
    return;

  views::MenuButton* button = static_cast<views::MenuButton*>(source);
  int id = button->tag();
  ui::MenuModel::ItemType type = menu_model_->GetTypeAt(id);
  if (type != ui::MenuModel::TYPE_SUBMENU)
    return;

  menu_delegate_.reset(new MenuDelegate(this));
  menu_delegate_->RunMenu(menu_model_->GetSubmenuModelAt(id), button);
}

}  // namespace atom
