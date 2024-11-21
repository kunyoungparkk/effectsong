import { useState, useEffect } from 'react';
import { NativeSelect, TextField, Grid } from "@mui/material";
import ScatterPlotIcon from '@mui/icons-material/ScatterPlot';
import CategoryIcon from '@mui/icons-material/Category';
import useUtil from '../Util';

const ShaderSettings = ({ module }) => {
    const [primitiveMode, setPrimitiveMode] = useState(0);
    const [vertexCount, setVertexCount] = useState(10000);

    const primitiveTypes = ['POINTS', 'LINES', 'LINE_LOOP', 'LINE_STRIP', 'TRIANGLES', 'TRI_STRIP', 'TRI_FAN'];

    const Util = useUtil();
    useEffect(() => {
        if (!module) { return; }
        let artShader = module.ArtShader.getInstance();
        setPrimitiveMode(artShader.getPrimitiveMode());
        setVertexCount(artShader.getVertexCount());
    }, [module])
    return (
        <Grid container
            sx={{
                height: "40px",
                backgroundColor: '#2e2e2e',
                pointerEvents: "auto",
                textAlign: "left",
                paddingLeft: '15px',
                paddingTop: '3px'
            }}
        >
            <Grid item xs={0.4} key="primitive-select-icon">
                <CategoryIcon sx={{ fontSize: "20px", paddingTop: "7px" }} />
            </Grid>
            <Grid item xs={1.3} key="primitive-select">
                <NativeSelect
                    value={primitiveMode}
                    inputProps={{
                        name: 'Primitive',
                        id: 'primitive-select',
                    }}
                    color="secondary"
                    onChange={(e) => {
                        console.log(e.target.value);
                        module.ArtShader.getInstance().setPrimitiveMode(e.target.value);
                        setPrimitiveMode(e.target.value);
                    }}
                >
                    {
                        primitiveTypes.map((value, index) => {
                            return <option value={index} key={index}>{value}</option>
                        })
                    }
                </NativeSelect>
            </Grid>
            <Grid item xs={0.5} key="vertex-count-icon">
                <ScatterPlotIcon sx={{ fontSize: "20px", paddingTop: "7px" }} />
            </Grid>
            <Grid item xs={1.5} key="vertex-count">
                <TextField
                    type="number"
                    variant="standard"
                    value={vertexCount}
                    onChange={(e) => {
                        if (Util.isValidNum(e.target.value)) {
                            const intValue = parseInt(e.target.value);
                            module.ArtShader.getInstance().setVertexCount(intValue);
                            setVertexCount(intValue);
                        }
                    }}
                    color="secondary"
                />
            </Grid>
        </Grid>
    );
}

export default ShaderSettings;