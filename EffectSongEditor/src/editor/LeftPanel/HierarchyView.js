import * as React from 'react';
import { useState } from 'react';
import { SimpleTreeView } from '@mui/x-tree-view';
import { useTreeItem2Utils } from '@mui/x-tree-view/hooks';
import { TreeItem2 } from '@mui/x-tree-view/TreeItem2';

const CustomTreeItem = React.forwardRef(function MyTreeItem(props, ref) {
  const { interactions } = useTreeItem2Utils({
    itemId: props.itemId,
    children: props.children,
  });

  const handleContentClick = (event) => {
    event.defaultMuiPrevented = true;
    interactions.handleSelection(event);
  };

  const handleIconContainerClick = (event) => {
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
      sx={{
        "& .Mui-selected": {
          backgroundColor: "transparent"
        },
        "& .MuiTreeItem-content": {
          height: "32px"
        }
      }}
    />
  );
});

const HierarchyView = ({ hierarchyData, selectCallback, expandIdList, setExpandIdList }) => {
  const [selectedItems, setSelectedItems] = useState([]);
  const getStyle = (isSelected) => {
    if (isSelected) {
        return { color: "white" };
    }else {
      return { color: "#868686" };
    }
  }
  const renderTree = (curNode) => (
    <CustomTreeItem itemId={curNode.id} key={curNode.id} label={curNode.name ? curNode.name : "Unnamed"}
      style={getStyle(curNode.isSelected)}
    >
      {Array.isArray(curNode.children)
        ? curNode.children.map((node) => renderTree(node))
        : null}
    </CustomTreeItem>
  );

  const handleKeyDown = (event) => {
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
      selectedItems={selectedItems}
      onItemSelectionToggle={selectCallback}
      onExpandedItemsChange={(e, itemIds) => {
        setExpandIdList(itemIds);
      }}
      onSelectedItemsChange={(event, ids) => {
        setSelectedItems(ids);
      }}
      expandedItems={expandIdList}
      onKeyDown={(e) => handleKeyDown(e)}
    >
      {hierarchyData.map((node)=>renderTree(node))}
      {/* {hierarchyData.length !== 0 ? renderTree(hierarchyData) : null} */}
    </SimpleTreeView>
  );
};
export default HierarchyView;