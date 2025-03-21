import * as React from 'react';
import { styled } from '@mui/material/styles';
import Tabs from '@mui/material/Tabs';
import Tab from '@mui/material/Tab';
import Box from '@mui/material/Box';
import HierarchyView from './HierarchyView';
import SettingsView from './SettingsView';

interface StyledTabsProps {
  children?: React.ReactNode;
  value: number;
  onChange: (event: React.SyntheticEvent, newValue: number) => void;
}

const StyledTabs = styled((props: StyledTabsProps) => (
  <Tabs {...props} TabIndicatorProps={{ children: <span className="MuiTabs-indicatorSpan" /> }} />
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

const StyledTab = styled((props: StyledTabProps) => <Tab disableRipple {...props} />)(({ theme }) => ({
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

export default function LeftTab() {
  const [value, setValue] = React.useState(0);
  const [expandIdList, setExpandIdList] = React.useState<Array<string>>([]);

  const handleChange = (event: React.SyntheticEvent, newValue: number) => {
    setValue(newValue);
  };

  const getPage = (index: number) => {
    switch (index) {
      case 0:
        return (
          <HierarchyView expandIdList={expandIdList} setExpandIdList={setExpandIdList} />
        );
      case 1:
        return <SettingsView />;
      default:
        return null;
    }
  };

  return (
    <Box>
      <Box sx={{ width: '100%', height: '70px' }}>
        <StyledTabs value={value} onChange={handleChange} aria-label="styled tabs example">
          <StyledTab label="Hierarchy" />
          <StyledTab label="Settings" />
        </StyledTabs>
        <Box sx={{ p: 3 }} />
      </Box>
      {getPage(value)}
    </Box>
  );
}
