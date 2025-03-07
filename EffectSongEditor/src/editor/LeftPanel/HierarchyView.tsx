import * as React from 'react';
import { useState } from 'react';
import { SimpleTreeView } from '@mui/x-tree-view';
import { useTreeItem2Utils } from '@mui/x-tree-view/hooks';
import { TreeItem2, TreeItem2Props } from '@mui/x-tree-view/TreeItem2';
import type {hierarchyNodeType} from './HierarchyView.d';
import { MuiEvent } from '@mui/x-tree-view/internals/models/events';

const CustomTreeItem = React.forwardRef(function MyTreeItem(props: TreeItem2Props, ref: React.Ref<HTMLLIElement>) {
  const { interactions } = useTreeItem2Utils({
    itemId: props.itemId,
    children: props.children,
  });

  const handleContentClick = (event: MuiEvent<React.MouseEvent<HTMLDivElement>>) => {
    event.defaultMuiPrevented = true;
    interactions.handleSelection(event);
  };

  const handleIconContainerClick = (event: React.MouseEvent<HTMLDivElement>) => {
    interactions.handleExpansion(event);
  };

  return (
    <TreeItem2
      {...props}
      ref={ref}
      slotProps={{
        content: { onClick: handleContentClick },
        iconContainer: { onClick: handleIconContainerClick }
      }}
    // sx={{
    //   "& .Mui-selected": {
    //     backgroundColor: "transparent"
    //   },
    //   "& .MuiTreeItem-content": {
    //     height: "32px"
    //   }
    // }}
    />
  );
});

type hierarchyViewType = {
  hierarchyData: Array<hierarchyNodeType>, 
  selectCallback: (event: React.SyntheticEvent, id: string, isSelected: boolean) => void, 
  expandIdList: string[], 
  setExpandIdList: (itemIds: string[]) => void
}

const HierarchyView = ({ hierarchyData, selectCallback, expandIdList, setExpandIdList }: hierarchyViewType) => {
  const [selectedItem, setSelectedItem] = useState<string>("");
  const getStyle = (isSelected: boolean) => {
    if (isSelected) {
      return { color: "white" };
    } else {
      return { color: "#868686" };
    }
  }
  const renderTree = (curNode: hierarchyNodeType) => (
    <CustomTreeItem itemId={curNode.id} key={curNode.id} label={curNode.name ? curNode.name : "Unnamed"}
      style={getStyle(curNode.isSelected)}
    >
      {Array.isArray(curNode.children)
        ? curNode.children.map((node: hierarchyNodeType) => renderTree(node))
        : null}
    </CustomTreeItem>
  );

  const handleKeyDown = (event: React.KeyboardEvent<HTMLUListElement>) => {
    if (event.key === ']') {
      document.dispatchEvent(new Event('hierarchyEventKeyUp'));
    } else if (event.key === '[') {
      document.dispatchEvent(new Event('hierarchyEventKeyDown'));
    }
  }

  return (
    <SimpleTreeView
      aria-label="HierarchyView"
      id="Hierarchy"
      sx={{
        height: 'calc(100vh - 120px)', flexGrow: 1, maxWidth: 300, overflowY: 'auto', scrollbarWidth: 'thin', scrollbarColor: "#868686 #2e2e2e"
      }}
      selectedItems={selectedItem}
      onItemSelectionToggle={selectCallback}
      onExpandedItemsChange={(e, itemIds) => {
        setExpandIdList(itemIds);
      }}
      onSelectedItemsChange={(event: React.SyntheticEvent, id: string | null) => {
        setSelectedItem(id as string);
      }}
      expandedItems={expandIdList}
      onKeyDown={(e) => handleKeyDown(e)}
    >
      {hierarchyData.map((node: hierarchyNodeType) => renderTree(node))}
      {/* {hierarchyData.length !== 0 ? renderTree(hierarchyData) : null} */}
    </SimpleTreeView>
  );
};
export default HierarchyView;