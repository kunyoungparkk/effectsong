import * as React from 'react';
import {useState} from 'react';
import { styled } from '@mui/material/styles';
import Tabs from '@mui/material/Tabs';
import Tab from '@mui/material/Tab';
import Box from '@mui/material/Box';
import NodeView from './NodeView.js';
import SceneView from './SceneView.js';

const StyledTabs = styled((props) => (
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

const StyledTab = styled((props) => <Tab disableRipple {...props} />)(
    ({ theme }) => ({
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
    }),
);

export default function RightTab({ updateHierarchy, module, targetNode }) {
    const [value, setValue] = React.useState(0);
    const handleChange = (event, newValue) => {
        setValue(newValue);
    };

    const getPage = (index) => {
        if(!targetNode){
            return <></>
        }
        switch(index){
            case 0:
                if(targetNode?.$$.ptrType.name === "Node*"){
                    return <NodeView module={module} targetNode={targetNode} updateHierarchy={updateHierarchy}/>
                }else if(targetNode?.$$.ptrType.name === "Scene*"){
                    return <SceneView module={module} targetScene={targetNode} updateHierarchy={updateHierarchy}/>
                }
            case 1:
                return <></>
            case 2:
                return <></>
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