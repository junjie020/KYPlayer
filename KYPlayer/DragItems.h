#ifndef _DRAGITEMS_H_
#define _DRAGITEMS_H_

namespace KY_UI
{
	class DragPlayListItemsHelper
	{
	public:
		DragPlayListItemsHelper();
		~DragPlayListItemsHelper();

		void SetListCtrl(CListCtrl *pList);

		void BeginDrag(KY::int32 idx, const CPoint &point);

		void Dragging(const CPoint &point);

		bool IsDragging() const;

		KY::uint32 GetDropIdx() const;

		void EndDrag();
		//void DropItemOnList(CListCtrl* pDropList);

	private:
		void Clear();


	private:
		CImageList* m_pDragImage;	//For creating and managing the drag-image
		
		KY::int32	m_nDragIndex;	//Index of selected item in the List we are dragging FROM
		KY::int32	m_nDropIndex;	//Index at which to drop item in the List we are dropping ON
		CListCtrl*	m_pPlayList;	//Pointer to window we are dropping on (will be cast to CListCtrl* type)

		bool		m_bDragging;	//T during a drag operation

	};

	inline bool DragPlayListItemsHelper::IsDragging() const
	{
		return m_bDragging;
	}

	inline void DragPlayListItemsHelper::SetListCtrl(CListCtrl *pList)
	{
		m_pPlayList = pList;
	}

	inline KY::uint32 DragPlayListItemsHelper::GetDropIdx() const
	{
		return m_nDropIndex;
	}

}

#endif //_DRAGITEMS_H_