import * as React from 'react';
import { styled } from '@mui/material/styles';
import Tabs from '@mui/material/Tabs';
import Tab from '@mui/material/Tab';
import Box from '@mui/material/Box';
import HierarchyView from './HierarchyView';
import type {hierarchyNodeType} from './HierarchyView.d';
import SettingsView from './SettingsView';
import * as core from '../../core/effectsong-core';

interface StyledTabsProps {
  children?: React.ReactNode;
  value: number;
  onChange: (event: React.SyntheticEvent, newValue: number) => void;
}

const StyledTabs = styled((props: StyledTabsProps) => (
  <Tabs
    {...props}
    TabIndicatorProps={{ children: <span className="MuiTabs-indicatorSpan" /> }}
  />
))({
  '& .MuiTabs-indicator': {
    display: 'flex',
    justifyContent: 'center',
    backgroundColor: 'transparent',
  },
  '& .MuiTabs-indicatorSpan': {
    maxWidth: 40,
    width: '100%',
    backgroundColor: '#868686',
  },
});

interface StyledTabProps {
  label: string;
}

const StyledTab = styled((props: StyledTabProps) => (
  <Tab disableRipple {...props} />
))(({ theme }) => ({
  textTransform: 'none',
  fontWeight: theme.typography.fontWeightRegular,
  fontSize: theme.typography.pxToRem(12),
  marginRight: theme.spacing(0),
  color: '#868686',
  '&.Mui-selected': {
    color: '#fff',
  },
  '&.Mui-focusVisible': {
    backgroundColor: 'rgba(100, 95, 228, 0.32)',
  },
}));

type leftTabProps = {
  module: core.MainModule,
  hierarchyData: Array<hierarchyNodeType>,
  selectCallback: (event: React.SyntheticEvent, id: string, isSelected: boolean) => void,
  expandIdList: string[],
  setExpandIdList: (itemIds: string[]) => void,
  targetNode: core.Node | null
}

export default function LeftTab({ module, hierarchyData, selectCallback, expandIdList, setExpandIdList, targetNode }: leftTabProps) {
  const [value, setValue] = React.useState(0);

  const handleChange = (event: React.SyntheticEvent, newValue: number) => {
    setValue(newValue);
  };

  const getPage = (index: number) => {
    switch (index) {
      case 0:
        return <HierarchyView
          hierarchyData={hierarchyData}
          selectCallback={selectCallback}
          expandIdList={expandIdList}
          setExpandIdList={setExpandIdList}
        />
      case 1:
        return <SettingsView module={module} targetNode={targetNode} />
      default:
        return null;
    }
  }

  return (
    <Box>
      <Box sx={{ width: '100%', height: '70px' }}>
        <StyledTabs
          value={value}
          onChange={handleChange}
          aria-label="styled tabs example"
        >
          <StyledTab label="Hierarchy" />
          <StyledTab label="Settings" />
        </StyledTabs>
        <Box sx={{ p: 3 }} />
      </Box>
      {getPage(value)}
    </Box>
  );
}