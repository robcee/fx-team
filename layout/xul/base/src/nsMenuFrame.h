/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

//
// nsMenuFrame
//

#ifndef nsMenuFrame_h__
#define nsMenuFrame_h__

#include "nsIAtom.h"
#include "nsCOMPtr.h"

#include "nsBoxFrame.h"
#include "nsFrameList.h"
#include "nsGkAtoms.h"
#include "nsMenuParent.h"
#include "nsXULPopupManager.h"
#include "nsITimer.h"
#include "nsIContent.h"
#include "mozilla/Attributes.h"

nsIFrame* NS_NewMenuFrame(nsIPresShell* aPresShell, nsStyleContext* aContext);
nsIFrame* NS_NewMenuItemFrame(nsIPresShell* aPresShell, nsStyleContext* aContext);

class nsMenuBarFrame;

#define NS_STATE_ACCELTEXT_IS_DERIVED  NS_STATE_BOX_CHILD_RESERVED

// the type of menuitem
enum nsMenuType {
  // a normal menuitem where a command is carried out when activated
  eMenuType_Normal = 0,
  // a menuitem with a checkmark that toggles when activated
  eMenuType_Checkbox = 1,
  // a radio menuitem where only one of it and its siblings with the same
  // name attribute can be checked at a time
  eMenuType_Radio = 2
};

enum nsMenuListType {
  eNotMenuList,      // not a menulist
  eReadonlyMenuList, // <menulist/>
  eEditableMenuList  // <menulist editable="true"/>
};

class nsMenuFrame;

/**
 * nsMenuTimerMediator is a wrapper around an nsMenuFrame which can be safely
 * passed to timers. The class is reference counted unlike the underlying
 * nsMenuFrame, so that it will exist as long as the timer holds a reference
 * to it. The callback is delegated to the contained nsMenuFrame as long as
 * the contained nsMenuFrame has not been destroyed.
 */
class nsMenuTimerMediator MOZ_FINAL : public nsITimerCallback
{
public:
  nsMenuTimerMediator(nsMenuFrame* aFrame);
  ~nsMenuTimerMediator();

  NS_DECL_ISUPPORTS
  NS_DECL_NSITIMERCALLBACK

  void ClearFrame();

private:

  // Pointer to the wrapped frame.
  nsMenuFrame* mFrame;
};

class nsMenuFrame : public nsBoxFrame
{
public:
  nsMenuFrame(nsIPresShell* aShell, nsStyleContext* aContext);

  NS_DECL_QUERYFRAME_TARGET(nsMenuFrame)
  NS_DECL_QUERYFRAME
  NS_DECL_FRAMEARENA_HELPERS

  NS_IMETHOD DoLayout(nsBoxLayoutState& aBoxLayoutState) MOZ_OVERRIDE;
  virtual nsSize GetMinSize(nsBoxLayoutState& aBoxLayoutState) MOZ_OVERRIDE;
  virtual nsSize GetPrefSize(nsBoxLayoutState& aBoxLayoutState) MOZ_OVERRIDE;

  NS_IMETHOD Init(nsIContent*      aContent,
                  nsIFrame*        aParent,
                  nsIFrame*        aPrevInFlow) MOZ_OVERRIDE;

#ifdef DEBUG_LAYOUT
  NS_IMETHOD SetDebug(nsBoxLayoutState& aState, bool aDebug) MOZ_OVERRIDE;
#endif

  // The following methods are all overridden so that the menupopup
  // can be stored in a separate list, so that it doesn't impact reflow of the
  // actual menu item at all.
  virtual const nsFrameList& GetChildList(ChildListID aList) const MOZ_OVERRIDE;
  virtual void GetChildLists(nsTArray<ChildList>* aLists) const MOZ_OVERRIDE;
  NS_IMETHOD SetInitialChildList(ChildListID     aListID,
                                 nsFrameList&    aChildList);
  virtual void DestroyFrom(nsIFrame* aDestructRoot);

  // Overridden to prevent events from going to children of the menu.
  NS_IMETHOD BuildDisplayListForChildren(nsDisplayListBuilder*   aBuilder,
                                         const nsRect&           aDirtyRect,
                                         const nsDisplayListSet& aLists) MOZ_OVERRIDE;
                                         
  // this method can destroy the frame
  NS_IMETHOD HandleEvent(nsPresContext* aPresContext, 
                         nsGUIEvent*     aEvent,
                         nsEventStatus*  aEventStatus) MOZ_OVERRIDE;

  NS_IMETHOD  AppendFrames(ChildListID     aListID,
                           nsFrameList&    aFrameList) MOZ_OVERRIDE;

  NS_IMETHOD  InsertFrames(ChildListID     aListID,
                           nsIFrame*       aPrevFrame,
                           nsFrameList&    aFrameList) MOZ_OVERRIDE;

  NS_IMETHOD  RemoveFrame(ChildListID     aListID,
                          nsIFrame*       aOldFrame) MOZ_OVERRIDE;

  virtual nsIAtom* GetType() const MOZ_OVERRIDE { return nsGkAtoms::menuFrame; }

  NS_IMETHOD SelectMenu(bool aActivateFlag);

  virtual nsIScrollableFrame* GetScrollTargetFrame();

  /**
   * NOTE: OpenMenu will open the menu asynchronously.
   */
  void OpenMenu(bool aSelectFirstItem);
  // CloseMenu closes the menu asynchronously
  void CloseMenu(bool aDeselectMenu);

  bool IsChecked() { return mChecked; }

  NS_IMETHOD GetActiveChild(nsIDOMElement** aResult);
  NS_IMETHOD SetActiveChild(nsIDOMElement* aChild);

  // called when the Enter key is pressed while the menuitem is the current
  // one in its parent popup. This will carry out the command attached to
  // the menuitem. If the menu should be opened, this frame will be returned,
  // otherwise null will be returned.
  nsMenuFrame* Enter(nsGUIEvent* aEvent);

  virtual void SetParent(nsIFrame* aParent);

  virtual nsMenuParent *GetMenuParent() { return mMenuParent; }
  const nsAString& GetRadioGroupName() { return mGroupName; }
  nsMenuType GetMenuType() { return mType; }
  nsMenuPopupFrame* GetPopup();

  /**
   * @return true if this frame has a popup child frame.
   */
  bool HasPopup() const
  {
    return (GetStateBits() & NS_STATE_MENU_HAS_POPUP_LIST) != 0;
  }


  // nsMenuFrame methods 

  bool IsOnMenuBar() { return mMenuParent && mMenuParent->IsMenuBar(); }
  bool IsOnActiveMenuBar() { return IsOnMenuBar() && mMenuParent->IsActive(); }
  virtual bool IsOpen();
  virtual bool IsMenu();
  nsMenuListType GetParentMenuListType();
  bool IsDisabled();
  void ToggleMenuState();

  // indiciate that the menu's popup has just been opened, so that the menu
  // can update its open state. This method modifies the open attribute on
  // the menu, so the frames could be gone after this call.
  void PopupOpened();
  // indiciate that the menu's popup has just been closed, so that the menu
  // can update its open state. The menu should be unhighlighted if
  // aDeselectedMenu is true. This method modifies the open attribute on
  // the menu, so the frames could be gone after this call.
  void PopupClosed(bool aDeselectMenu);

  // returns true if this is a menu on another menu popup. A menu is a submenu
  // if it has a parent popup or menupopup.
  bool IsOnMenu() { return mMenuParent && mMenuParent->IsMenu(); }
  void SetIsMenu(bool aIsMenu) { mIsMenu = aIsMenu; }

#ifdef DEBUG
  NS_IMETHOD GetFrameName(nsAString& aResult) const MOZ_OVERRIDE
  {
      return MakeFrameName(NS_LITERAL_STRING("Menu"), aResult);
  }
#endif

  static bool IsSizedToPopup(nsIContent* aContent, bool aRequireAlways);

protected:
  friend class nsMenuTimerMediator;
  friend class nsASyncMenuInitialization;
  friend class nsMenuAttributeChangedEvent;

  /**
   * Initialize the popup list to the first popup frame within
   * aChildList. Removes the popup, if any, from aChildList.
   */
  void SetPopupFrame(nsFrameList& aChildList);

  /**
   * Get the popup frame list from the frame property.
   * @return the property value if it exists, nullptr otherwise.
   */
  nsFrameList* GetPopupList() const;

  /**
   * Destroy the popup list property.  The list must exist and be empty.
   */
  void DestroyPopupList();

  // set mMenuParent to the nearest enclosing menu bar or menupopup frame of
  // aParent (or aParent itself). This is called when initializing the frame,
  // so aParent should be the expected parent of this frame.
  void InitMenuParent(nsIFrame* aParent);

  // Update the menu's type (normal, checkbox, radio).
  // This method can destroy the frame.
  void UpdateMenuType(nsPresContext* aPresContext);
  // Update the checked state of the menu, and for radios, clear any other
  // checked items. This method can destroy the frame.
  void UpdateMenuSpecialState(nsPresContext* aPresContext);

  // Examines the key node and builds the accelerator.
  void BuildAcceleratorText(bool aNotify);

  // Called to execute our command handler. This method can destroy the frame.
  void Execute(nsGUIEvent *aEvent);

  // This method can destroy the frame
  NS_IMETHOD AttributeChanged(int32_t aNameSpaceID,
                              nsIAtom* aAttribute,
                              int32_t aModType) MOZ_OVERRIDE;
  virtual ~nsMenuFrame() { }

  bool SizeToPopup(nsBoxLayoutState& aState, nsSize& aSize);

  bool ShouldBlink();
  void StartBlinking(nsGUIEvent *aEvent, bool aFlipChecked);
  void StopBlinking();
  void CreateMenuCommandEvent(nsGUIEvent *aEvent, bool aFlipChecked);
  void PassMenuCommandEventToPopupManager();

protected:
#ifdef DEBUG_LAYOUT
  nsresult SetDebug(nsBoxLayoutState& aState, nsIFrame* aList, bool aDebug);
#endif
  NS_HIDDEN_(nsresult) Notify(nsITimer* aTimer);

  bool mIsMenu; // Whether or not we can even have children or not.
  bool mChecked;              // are we checked?
  bool mIgnoreAccelTextChange; // temporarily set while determining the accelerator key
  nsMenuType mType;

  nsMenuParent* mMenuParent; // Our parent menu.

  // Reference to the mediator which wraps this frame.
  nsRefPtr<nsMenuTimerMediator> mTimerMediator;

  nsCOMPtr<nsITimer> mOpenTimer;
  nsCOMPtr<nsITimer> mBlinkTimer;

  uint8_t mBlinkState; // 0: not blinking, 1: off, 2: on
  nsRefPtr<nsXULMenuCommandEvent> mDelayedMenuCommandEvent;

  nsString mGroupName;

}; // class nsMenuFrame

#endif
