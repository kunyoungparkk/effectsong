import * as React from 'react';
import { styled } from '@mui/material/styles';
import Tabs from '@mui/material/Tabs';
import Tab from '@mui/material/Tab';
import Box from '@mui/material/Box';
import NodeView from './NodeView';
import VisualScriptView from './VisualScriptView';
import { selectedNodeAtom } from '../atom';
import { useAtomValue } from 'jotai';

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
    color: 'white'
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
  marginRight: theme.spacing(-1),
  color: '#868686',
  '&.Mui-selected': {
    color: 'rgba(255, 255, 255, 0.7)',
  },
  '&.Mui-focusVisible': {
    backgroundColor: 'rgba(100, 95, 228, 0.32)',
  },
}));

type rightTabProps = {
  updateHierarchy: () => void
}

export default function RightTab({ updateHierarchy }: rightTabProps) {
  const selectedNode = useAtomValue(selectedNodeAtom);
  const [value, setValue] = React.useState(0);
  const handleChange = (event: React.SyntheticEvent, newValue: number) => {
    setValue(newValue);
  };

  const getPage = (index: number) : JSX.Element => {
    if(!selectedNode){
      return <></>;
    }
    switch (index) {
      case 0:
        return <NodeView updateHierarchy={updateHierarchy} />
      case 1:
        return <VisualScriptView />
      default:
        return <></>;
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
          <StyledTab label="Node" />
          <StyledTab label="Visual Script" />
        </StyledTabs>
        <Box sx={{ p: 3 }} />
      </Box>
      {getPage(value)}
    </Box>
  );
}