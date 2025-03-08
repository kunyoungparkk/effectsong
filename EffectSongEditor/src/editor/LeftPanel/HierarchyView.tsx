import * as React from 'react';
import { useState, useCallback } from 'react';
import { SimpleTreeView } from '@mui/x-tree-view';
import { useTreeItem2Utils } from '@mui/x-tree-view/hooks';
import { TreeItem2, TreeItem2Props } from '@mui/x-tree-view/TreeItem2';
import type { hierarchyNodeType } from '../common';
import { MuiEvent } from '@mui/x-tree-view/internals/models/events';
import CoreManager from '../CoreManager';
import * as core from '../../core/effectsong-core';
import { selectedNodeAtom, hierarchyDataAtom } from '../atom';
import { useSetAtom, useAtomValue } from 'jotai';

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
        iconContainer: { onClick: handleIconContainerClick },
      }}
    />
  );
});

type hierarchyViewType = {
  expandIdList: string[];
  setExpandIdList: (itemIds: string[]) => void;
};

const HierarchyView = ({ expandIdList, setExpandIdList }: hierarchyViewType) => {
  const [selectedItem, setSelectedItem] = useState<string>('');
  const setSelectedNode = useSetAtom(selectedNodeAtom);
  const hierarchyData = useAtomValue(hierarchyDataAtom);

  const getStyle = (isSelected: boolean) => {
    if (isSelected) {
      return { color: 'white' };
    } else {
      return { color: '#868686' };
    }
  };
  const renderTree = (curNode: hierarchyNodeType) => (
    <CustomTreeItem
      itemId={curNode.id}
      key={curNode.id}
      label={curNode.name ? curNode.name : 'Unnamed'}
      style={getStyle(curNode.id === selectedItem)}>
      {Array.isArray(curNode.children) ? curNode.children.map((node: hierarchyNodeType) => renderTree(node)) : null}
    </CustomTreeItem>
  );

  const handleKeyDown = (event: React.KeyboardEvent<HTMLUListElement>) => {
    if (event.key === ']') {
      document.dispatchEvent(new Event('hierarchyEventKeyUp'));
    } else if (event.key === '[') {
      document.dispatchEvent(new Event('hierarchyEventKeyDown'));
    }
  };

  const getNodeById = useCallback((id: string) => {
    const idxList = id.split('-');

    let curNode: core.Node = CoreManager.getInstance().getRenderer().getSceneAt(parseInt(idxList[0]))!;
    for (let i = 1; i < idxList.length; i++) {
      curNode = curNode.getChildAt(parseInt(idxList[i]))!;
    }
    return curNode;
  }, []);

  return (
    <SimpleTreeView
      aria-label="HierarchyView"
      id="Hierarchy"
      sx={{
        height: 'calc(100vh - 120px)',
        flexGrow: 1,
        maxWidth: 300,
        overflowY: 'auto',
        scrollbarWidth: 'thin',
        scrollbarColor: '#868686 #2e2e2e',
      }}
      selectedItems={selectedItem}
      onItemSelectionToggle={(event: React.SyntheticEvent, id: string, isSelected: boolean) => {
        if (!isSelected) {
          return;
        }
        setSelectedNode(getNodeById(id));
      }}
      onExpandedItemsChange={(e, itemIds) => {
        setExpandIdList(itemIds);
      }}
      onSelectedItemsChange={(event: React.SyntheticEvent, id: string | null) => {
        setSelectedItem(id as string);
      }}
      expandedItems={expandIdList}
      onKeyDown={e => handleKeyDown(e)}>
      {hierarchyData.map((node: hierarchyNodeType) => renderTree(node))}
      {/* {hierarchyData.length !== 0 ? renderTree(hierarchyData) : null} */}
    </SimpleTreeView>
  );
};
export default HierarchyView;
