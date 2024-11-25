import { useEffect, useState } from 'react'
import { TextField, Grid } from '@mui/material';
import useUtil from '../Util';

const LightView = ({ targetNode }) => {
    const Util = useUtil();
    const light = targetNode?.getLight();

    const [name, setName] = useState("");
    const [color, setColor] = useState([0.0, 0.0, 0.0]);
    const [intensity, setIntensity] = useState(0.0);
    const [range, setRange] = useState(0.0);
    const [innerConeAngle, setInnerConeAngle] = useState(0.0);
    const [outerConeAngle, setOuterConeAngle] = useState(0.0);

    useEffect(() => {
        if (!light) {
            return;
        }
        setName(light.name);
        const glmColor = light.color;
        setColor(Util.roundToNearestStep(glmColor.x), Util.roundToNearestStep(glmColor.y), Util.roundToNearestStep(glmColor.z));
        setIntensity(Util.roundToNearestStep(light.intensity));
        setRange(Util.roundToNearestStep(light.range));
        setInnerConeAngle(Util.roundToNearestStep(light.innerConeAngle));
        setOuterConeAngle(Util.roundToNearestStep(light.outerConeAngle));
    }, [targetNode]);

    const getView = () => {
        if (!light) {
            return (
                <Grid container
                    rowSpacing={2}
                    columnSpacing={{ xs: 1, sm: 2, md: 3 }}
                    sx={{
                        padding: 1,
                        color: "#868686"
                    }}
                >
                    <Grid item xs={12}>No Light
                    </Grid>
                </Grid>);
        }
        else {
            <Grid container
                rowSpacing={2}
                columnSpacing={{ xs: 1, sm: 2, md: 3 }}
                sx={{
                    padding: 1,
                    color: "#868686"
                }}
            >
                <Grid item xs={12}>Color
                </Grid>
                <Grid item xs={4}>
                    <TextField
                        type="number"
                        variant="standard"
                        id="color-r"
                        label="r"
                        value={color[0]}
                        onChange={(e) => {
                            let tempColor = light.getColor();
                            if (Util.isValidNum(e.target.value)) {
                                tempColor.x = parseFloat(e.target.value);
                                light.color =tempColor;

                                setColor([tempColor.x, tempColor.y, tempColor.z]);
                            } else {
                                setColor([e.target.value, tempColor.y, tempColor.z]);
                            }
                        }}
                        inputProps={{ style: { color: "#868686" } }}
                    />
                </Grid>
                <Grid item xs={4}>
                    <TextField
                        type="number"
                        variant="standard"
                        id="color-g"
                        label="g"
                        value={color[1]}
                        onChange={(e) => {
                            let tempColor = light.getColor();
                            if (Util.isValidNum(e.target.value)) {
                                tempColor.y = parseFloat(e.target.value);
                                light.color =tempColor;

                                setColor([tempColor.x, tempColor.y, tempColor.z]);
                            } else {
                                setColor([tempColor.x, e.target.value, tempColor.z]);
                            }
                        }}
                        inputProps={{ style: { color: "#868686" } }}
                    />
                </Grid>
                <Grid item xs={4}>
                    <TextField
                        type="number"
                        variant="standard"
                        id="color-b"
                        label="b"
                        value={color[2]}
                        onChange={(e) => {
                            let tempColor = light.getColor();
                            if (Util.isValidNum(e.target.value)) {
                                tempColor.z = parseFloat(e.target.value);
                                light.color =tempColor;

                                setColor([tempColor.x, tempColor.y, tempColor.z]);
                            } else {
                                setColor([tempColor.x, tempColor.y, e.target.value]);
                            }
                        }}
                        inputProps={{ style: { color: "#868686" } }}
                    />
                </Grid>
                <Grid item xs={12}>
                    <TextField
                        type="number"
                        variant="standard"
                        id="intensity-input"
                        label="intensity"
                        value={intensity}
                        onChange={(e) => {
                            if (Util.isValidNum(e.target.value)) {
                                const floatValue = parseFloat(e.target.value);
                                light.intensity = floatValue;
                                setIntensity(floatValue);
                            } else {
                                setIntensity(e.target.value);
                            }
                        }}
                        inputProps={{ style: { color: "#868686" } }}
                    />
                </Grid>
                <Grid item xs={12}>
                    <TextField
                        type="number"
                        variant="standard"
                        id="range-input"
                        label="range"
                        value={range}
                        onChange={(e) => {
                            if (Util.isValidNum(e.target.value)) {
                                const floatValue = parseFloat(e.target.value);
                                light.range = floatValue;
                                setRange(floatValue);
                            } else {
                                setRange(e.target.value);
                            }
                        }}
                        inputProps={{ style: { color: "#868686" } }}
                    />
                </Grid>
                <Grid item xs={12}>
                    <TextField
                        type="number"
                        variant="standard"
                        id="innerConeAngle-input"
                        label="innerConeAngle"
                        value={innerConeAngle}
                        onChange={(e) => {
                            if (Util.isValidNum(e.target.value)) {
                                const floatValue = parseFloat(e.target.value);
                                light.innerConeAngle = floatValue;
                                setInnerConeAngle(floatValue);
                            } else {
                                setInnerConeAngle(e.target.value);
                            }
                        }}
                        inputProps={{ style: { color: "#868686" } }}
                    />
                </Grid>
                <Grid item xs={12}>
                    <TextField
                        type="number"
                        variant="standard"
                        id="outerConeAngle-input"
                        label="outerConeAngle"
                        value={outerConeAngle}
                        onChange={(e) => {
                            if (Util.isValidNum(e.target.value)) {
                                const floatValue = parseFloat(e.target.value);
                                light.outerConeAngle = floatValue;
                                setOuterConeAngle(floatValue);
                            } else {
                                setOuterConeAngle(e.target.value);
                            }
                        }}
                        inputProps={{ style: { color: "#868686" } }}
                    />
                </Grid>

            </Grid>
        }
    }
    return (
        <>
            {getView()}
        </>
    );
}

export default LightView;