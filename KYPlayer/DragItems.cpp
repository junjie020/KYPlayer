#include "stdafx.h"

#include "DragItems.h"
#include "Resource.h"

namespace KY_UI
{
	DragPlayListItemsHelper::DragPlayListItemsHelper()
		: m_pPlayList(nullptr)
	{
		Clear();
	}

	DragPlayListItemsHelper::~DragPlayListItemsHelper()
	{
		m_pPlayList = nullptr;
		Clear();
	}

	void DragPlayListItemsHelper::BeginDrag(KY::int32 idx, const CPoint &point)
	{
		BOOST_ASSERT(m_pPlayList);

		CWnd *pParentWnd = m_pPlayList->GetParent();

		m_nDragIndex = idx;

		//// Create a drag image
		POINT pt;
		int nOffset = -10; //offset in pixels for drag image
		pt.x = nOffset;
		pt.y = nOffset;

		m_pDragImage = m_pPlayList->CreateDragImage(m_nDragIndex, &pt);
		ASSERT(m_pDragImage); //make sure it was created
		//We will call delete later (in LButtonUp) to clean this up

		CBitmap bitmap;
		if (m_pPlayList->GetSelectedCount() > 1) //more than 1 item in list is selected
			//bitmap.LoadBitmap(IDB_BITMAP_MULTI);
			bitmap.LoadBitmap(IDB_BITMAP_MULTI_BOXES);
		else
			bitmap.LoadBitmap(IDB_BITMAP_BOX);
		m_pDragImage->Replace(0, &bitmap, &bitmap);

		//// Change the cursor to the drag image
		////	(still must perform DragMove() in OnMouseMove() to show it moving)
		m_pDragImage->BeginDrag(0, CPoint(nOffset, nOffset - 4));
		m_pDragImage->DragEnter(pParentWnd->GetDesktopWindow(), point);

		//// Set dragging flag and others
		m_bDragging = true;	//we are in a drag and drop operation
		m_nDropIndex = -1;	//we don't have a drop index yet

		//// Capture all mouse messages
		pParentWnd->SetCapture();
	}

	void DragPlayListItemsHelper::Dragging(const CPoint &point)
	{
		//While the mouse is moving, this routine is called.
		//This routine will redraw the drag image at the present
		// mouse location to display the dragging.
		//Also, while over a CListCtrl, this routine will highlight
		// the item we are hovering over.

		//// If we are in a drag/drop procedure (m_bDragging is true)
		if (m_bDragging)
		{
			BOOST_ASSERT(m_pPlayList);
			CWnd *pParentWnd = m_pPlayList->GetParent();

			//// Move the drag image
			CPoint pt(point);	//get our current mouse coordinates
			pParentWnd->ClientToScreen(&pt); //convert to screen coordinates
			m_pDragImage->DragMove(pt); //move the drag image to those coordinates
			// Unlock window updates (this allows the dragging image to be shown smoothly)
			m_pDragImage->DragShowNolock(false);

			//// Get the CWnd pointer of the window that is under the mouse cursor
			CWnd* pDropWnd = pParentWnd->WindowFromPoint(pt);
			BOOST_ASSERT(pDropWnd); //make sure we have a window

			if (pDropWnd != m_pPlayList)
			{
				// Lock window updates
				SetCursor(LoadCursor(NULL, IDC_NO));
				m_pDragImage->DragShowNolock(true);
				return;
			}

			//Note that we can drop here
			SetCursor(LoadCursor(NULL, IDC_ARROW));
		
			if (-1 != m_nDropIndex)
			{
				m_pPlayList->SetItemState(m_nDropIndex, 0, LVIS_DROPHILITED);
				m_pPlayList->RedrawItems(m_nDropIndex, m_nDropIndex);
			}

			m_pPlayList->ScreenToClient(&pt);

			UINT uFlags;
			m_nDropIndex = m_pPlayList->HitTest(pt, &uFlags);
			
			m_pPlayList->SetItemState(m_nDropIndex, LVIS_DROPHILITED, LVIS_DROPHILITED);
			
			m_pPlayList->RedrawItems(m_nDropIndex, m_nDropIndex);
			m_pPlayList->UpdateWindow();

			m_pDragImage->DragShowNolock(true);

			////// If we drag outside current window we need to adjust the highlights displayed
			//if (pDropWnd != m_pPlayList)
			//{
			//	if (m_nDropIndex != -1) //If we drag over the CListCtrl header, turn off the hover highlight
			//	{
			//		TRACE(L"m_nDropIndex is -1\n");
			//		CListCtrl* pList = (CListCtrl*)m_pPlayList;
			//		VERIFY(pList->SetItemState(m_nDropIndex, 0, LVIS_DROPHILITED));
			//		// redraw item
			//		VERIFY(pList->RedrawItems(m_nDropIndex, m_nDropIndex));
			//		pList->UpdateWindow();
			//		m_nDropIndex = -1;
			//	}
			//	else //If we drag out of the CListCtrl altogether
			//	{
			//		TRACE(L"m_nDropIndex is not -1\n");
			//		CListCtrl* pList = (CListCtrl*)m_pPlayList;
			//		int i = 0;
			//		int nCount = pList->GetItemCount();
			//		for (i = 0; i < nCount; i++)
			//		{
			//			pList->SetItemState(i, 0, LVIS_DROPHILITED);
			//		}
			//		pList->RedrawItems(0, nCount);
			//		pList->UpdateWindow();
			//	}
			//}

			//// Convert from screen coordinates to drop target client coordinates
			//m_pPlayList->ScreenToClient(&pt);

			////If we are hovering over a CListCtrl we need to adjust the highlights
			//if (pDropWnd->IsKindOf(RUNTIME_CLASS(CListCtrl)))
			//{
			//	//Note that we can drop here
			//	SetCursor(LoadCursor(NULL, IDC_ARROW));
			//	UINT uFlags;
			//	CListCtrl* pList = (CListCtrl*)pDropWnd;

			//	// Turn off hilight for previous drop target
			//	pList->SetItemState(m_nDropIndex, 0, LVIS_DROPHILITED);
			//	// Redraw previous item
			//	pList->RedrawItems(m_nDropIndex, m_nDropIndex);

			//	// Get the item that is below cursor
			//	m_nDropIndex = ((CListCtrl*)pDropWnd)->HitTest(pt, &uFlags);
			//	// Highlight it
			//	pList->SetItemState(m_nDropIndex, LVIS_DROPHILITED, LVIS_DROPHILITED);
			//	// Redraw item
			//	pList->RedrawItems(m_nDropIndex, m_nDropIndex);
			//	pList->UpdateWindow();
			//}
			//else
			//{
			//	//If we are not hovering over a CListCtrl, change the cursor
			//	// to note that we cannot drop here
			//	SetCursor(LoadCursor(NULL, IDC_NO));
			//}
			// Lock window updates
			//m_pDragImage->DragShowNolock(true);
		}
	}

	//void DragPlayListItemsHelper::DropItemOnList(CListCtrl* pDropList)
	//{
	//	//This routine performs the actual drop of the item dragged.
	//	//It simply grabs the info from the Drag list (pDragList)
	//	// and puts that info into the list dropped on (pDropList).
	//	//Send:	pDragList = pointer to CListCtrl we dragged from,
	//	//		pDropList = pointer to CListCtrl we are dropping on.
	//	//Return: nothing.

	//	////Variables


	//	// Unhilight the drop target
	//	pDropList->SetItemState(m_nDropIndex, 0, LVIS_DROPHILITED);

	//	auto dropIdx = m_nDropIndex;

	//	std::list<KY::uint32>	allDropIdxs;
	//
	//	//Retrieve the selected items
	//	for (POSITION pos = pDropList->GetFirstSelectedItemPosition();
	//		pos;  )		
	//	{
	//		wchar_t buf[256] = { 0 };

	//		LVITEM item = { 0 };
	//		
	//		//ZeroMemory(pItem, sizeof(lvItem)); //If you use ZeroMemory on the lvItem struct, it creates an error when you try to set sCol2
	//		const auto curSelIdx = pDropList->GetNextSelectedItem(pos);
	//		item.iItem = curSelIdx;
	//		item.mask	= LVIF_TEXT;
	//		item.pszText = buf; //since this is a pointer to the string, we need a new pointer to a new string on the heap
	//		item.cchTextMax = 255;

	//		pDropList->GetItem(&item); //retrieve the information

	//		std::list<std::wstring>	colNames;

	//		const KY::uint32 columnCount = pDropList->GetHeaderCtrl()->GetItemCount();

	//		for (KY::uint32 ii = 1; ii < columnCount; ++ii)
	//		{
	//			colNames.push_back(LPCTSTR(pDropList->GetItemText(item.iItem, ii)));
	//		}

	//		pDropList->DeleteItem(curSelIdx);

	//		if (m_nDragIndex < m_nDropIndex)
	//			--dropIdx;

	//		allDropIdxs.push_back(dropIdx);

	//		item.iItem = dropIdx;
	//		pDropList->InsertItem(&item);

	//		auto colIdx = 1;
	//		for (auto it = colNames.begin(); it != colNames.end(); ++it, ++colIdx)
	//		{
	//			pDropList->SetItemText(item.iItem, colIdx, it->c_str());
	//		}
	//
	//		//pDropList->SetItemState(item.iItem, LVIS_SELECTED, LVIS_SELECTED); //highlight/select the item we just added

	//	} //EO while(pos) -- at this point we have deleted the moving items and stored them in memory


	//	for (auto it = allDropIdxs.begin(); it != allDropIdxs.end(); ++it)
	//	{
	//		pDropList->SetItemState(*it, LVIS_SELECTED, LVIS_SELECTED); //highlight/select the item we just added
	//	}
	//	//if (pDragList == pDropList) //we are reordering the list (moving)
	//	//{
	//	//	//Delete the selected items
	//	//	pos = pDragList->GetFirstSelectedItemPosition();
	//	//	while (pos)
	//	//	{
	//	//		pos = pDragList->GetFirstSelectedItemPosition();
	//	//		m_nDragIndex = pDragList->GetNextSelectedItem(pos);

	//	//		pDragList->DeleteItem(m_nDragIndex); //since we are MOVING, delete the item
	//	//		if (m_nDragIndex < m_nDropIndex) m_nDropIndex--; //must decrement the drop index to account
	//	//		//for the deleted items
	//	//	} //EO while(pos)
	//	//} //EO if(pDragList...

	//	////Iterate through the items stored in memory and add them back into the CListCtrl at the drop index
	//	//listPos = listItems.GetHeadPosition();
	//	//while (listPos)
	//	//{
	//	//	pItem = listItems.GetNext(listPos);

	//	//	m_nDropIndex = (m_nDropIndex == -1) ? pDropList->GetItemCount() : m_nDropIndex;
	//	//	pItem->plvi->iItem = m_nDropIndex;
	//	//	pDropList->InsertItem(pItem->plvi); //add the item
	//	//	pDropList->SetItemText(pItem->plvi->iItem, 1, pItem->sCol2);

	//	//	pDropList->SetItemState(pItem->plvi->iItem, LVIS_SELECTED, LVIS_SELECTED); //highlight/select the item we just added

	//	//	m_nDropIndex++; //increment the index we are dropping at to keep the dropped items in the same order they were in in the Drag List
	//	//	//If we dont' increment this, the items are added in reverse order
	//	//} //EO while(listPos)

	//	//if (pDropList->GetSelectedCount() == 1)
	//	//{
	//	//	//Set up the LV_ITEM for retrieving item from pDragList and adding the new item to the pDropList
	//	//	wchar_t szLabel[256];
	//	//	LVITEM lviT;

	//	//	ZeroMemory(&lviT, sizeof(LVITEM)); //allocate and clear memory space for LV_ITEM
	//	//	lviT.iItem = m_nDragIndex;
	//	//	lviT.mask = LVIF_TEXT;
	//	//	lviT.pszText = szLabel;
	//	//	lviT.cchTextMax = 255;

	//	//	pDropList->DeleteItem(m_nDragIndex);
	//	//	if (m_nDragIndex < m_nDropIndex) 
	//	//		m_nDropIndex--; //decrement drop index to account for item

	//	//	//being deleted above it

	//	//	// Get item that was dragged
	//	//	pDropList->GetItem(&lviT);
	//	//	CString ss = pDropList->GetItemText(lviT.iItem, 1);

	//	//	// Insert item into pDropList
	//	//	// if m_nDropIndex == -1, iItem = GetItemCount() (inserts at end of list), else iItem = m_nDropIndex
	//	//	lviT.iItem = (m_nDropIndex == -1) ? pDropList->GetItemCount() : m_nDropIndex;
	//	//	pDropList->InsertItem(&lviT);
	//	//	pDropList->SetItemText(lviT.iItem, 1, ss);

	//	//	// Select the new item we just inserted
	//	//	pDropList->SetItemState(lviT.iItem, LVIS_SELECTED, LVIS_SELECTED);
	//	//}
	//	//else //more than 1 item is being dropped
	//	//{
	//	//	//We have to parse through all of the selected items from the DragList
	//	//	//1) Retrieve the info for the items and store them in memory
	//	//	//2) If we are reordering, delete the items from the list
	//	//	//3) Insert the items into the list (either same list or different list)

	//	//	CList<lvItem*, lvItem*> listItems;
	//	//	POSITION listPos;

	//	//	//Retrieve the selected items
	//	//	POSITION pos = pDragList->GetFirstSelectedItemPosition(); //iterator for the CListCtrl
	//	//	while (pos) //so long as we have a valid POSITION, we keep iterating
	//	//	{
	//	//		plvitem = new LVITEM;
	//	//		ZeroMemory(plvitem, sizeof(LVITEM));
	//	//		pItem = new lvItem;
	//	//		//ZeroMemory(pItem, sizeof(lvItem)); //If you use ZeroMemory on the lvItem struct, it creates an error when you try to set sCol2
	//	//		pItem->plvi = plvitem;
	//	//		pItem->plvi->iItem = m_nDragIndex;
	//	//		pItem->plvi->mask = LVIF_TEXT;
	//	//		pItem->plvi->pszText = new wchar_t; //since this is a pointer to the string, we need a new pointer to a new string on the heap
	//	//		pItem->plvi->cchTextMax = 255;

	//	//		m_nDragIndex = pDragList->GetNextSelectedItem(pos);

	//	//		//Get the item
	//	//		pItem->plvi->iItem = m_nDragIndex; //set the index in the drag list to the selected item
	//	//		pDragList->GetItem(pItem->plvi); //retrieve the information
	//	//		pItem->sCol2 = pDragList->GetItemText(pItem->plvi->iItem, 1);

	//	//		//Save the pointer to the new item in our CList
	//	//		listItems.AddTail(pItem);
	//	//	} //EO while(pos) -- at this point we have deleted the moving items and stored them in memory

	//	//	if (pDragList == pDropList) //we are reordering the list (moving)
	//	//	{
	//	//		//Delete the selected items
	//	//		pos = pDragList->GetFirstSelectedItemPosition();
	//	//		while (pos)
	//	//		{
	//	//			pos = pDragList->GetFirstSelectedItemPosition();
	//	//			m_nDragIndex = pDragList->GetNextSelectedItem(pos);

	//	//			pDragList->DeleteItem(m_nDragIndex); //since we are MOVING, delete the item
	//	//			if (m_nDragIndex < m_nDropIndex) m_nDropIndex--; //must decrement the drop index to account
	//	//			//for the deleted items
	//	//		} //EO while(pos)
	//	//	} //EO if(pDragList...

	//	//	//Iterate through the items stored in memory and add them back into the CListCtrl at the drop index
	//	//	listPos = listItems.GetHeadPosition();
	//	//	while (listPos)
	//	//	{
	//	//		pItem = listItems.GetNext(listPos);

	//	//		m_nDropIndex = (m_nDropIndex == -1) ? pDropList->GetItemCount() : m_nDropIndex;
	//	//		pItem->plvi->iItem = m_nDropIndex;
	//	//		pDropList->InsertItem(pItem->plvi); //add the item
	//	//		pDropList->SetItemText(pItem->plvi->iItem, 1, pItem->sCol2);

	//	//		pDropList->SetItemState(pItem->plvi->iItem, LVIS_SELECTED, LVIS_SELECTED); //highlight/select the item we just added

	//	//		m_nDropIndex++; //increment the index we are dropping at to keep the dropped items in the same order they were in in the Drag List
	//	//		//If we dont' increment this, the items are added in reverse order
	//	//	} //EO while(listPos)

	//	//}
	//}




	void DragPlayListItemsHelper::EndDrag()
	{
		//This routine is the end of the drag/drop operation.
		//When the button is released, we are to drop the item.
		//There are a few things we need to do to clean up and
		// finalize the drop:
		//	1) Release the mouse capture
		//	2) Set m_bDragging to false to signify we are not dragging
		//	3) Actually drop the item (we call a separate function to do that)

		//If we are in a drag and drop operation (otherwise we don't do anything)
		if (m_bDragging)
		{
			BOOST_ASSERT(m_pPlayList);
			CWnd *pParentWnd = m_pPlayList->GetParent();
			// Release mouse capture, so that other controls can get control/messages
			ReleaseCapture();

			// End dragging image
			m_pDragImage->DragLeave(pParentWnd->GetDesktopWindow());
			m_pDragImage->EndDrag();
			delete m_pDragImage; //must delete it because it was created at the beginning of the drag
			m_pDragImage = nullptr;

			//CPoint pt(point); //Get current mouse coordinates
			//pParentWnd->ClientToScreen(&pt); //Convert to screen coordinates
			//// Get the CWnd pointer of the window that is under the mouse cursor
			//CWnd* pDropWnd = pParentWnd->WindowFromPoint(pt);
			//ASSERT(pDropWnd); //make sure we have a window pointer
			//// If window is CListCtrl, we perform the drop
			//if (pDropWnd->IsKindOf(RUNTIME_CLASS(CListCtrl)))
			//{
			//	DropItemOnList((CListCtrl*)pDropWnd); //Call routine to perform the actual drop
			//}

			Clear();
		}
	}

	void DragPlayListItemsHelper::Clear()
	{
		m_pDragImage = nullptr;
		m_bDragging = false;
		m_nDropIndex = m_nDragIndex = -1;
	}

}


