import { useEffect, useState } from 'react'
import { Box, TextField, Grid } from '@mui/material';
import useUtil from '../Util';
import CameraView from './CameraView';
import LightView from './LightView';
const NodeView = ({ module, targetNode }) => {
    const Util = useUtil();
    const [name, setName] = useState("");
    const [position, setPosition] = useState([0.0, 0.0, 0.0]);
    const [rotation, setRotation] = useState([0.0, 0.0, 0.0]);
    const [scale, setScale] = useState([0.0, 0.0, 0.0]);

    useEffect(() => {
        if (!module || !targetNode) {
            return;
        }
        const inputPos = targetNode.getPosition();
        const inputEuler = targetNode.getEulerRotation();
        const inputScale = targetNode.getScale();

        setName(targetNode.getName());
        setPosition([Util.roundToNearestStep(inputPos.x), Util.roundToNearestStep(inputPos.y), Util.roundToNearestStep(inputPos.z)]);
        setRotation([Util.roundToNearestStep(inputEuler.x), Util.roundToNearestStep(inputEuler.y), Util.roundToNearestStep(inputEuler.z)]);
        setScale([Util.roundToNearestStep(inputScale.x), Util.roundToNearestStep(inputScale.y), Util.roundToNearestStep(inputScale.z)]);
    }, [targetNode])
    return (
        <Box>
            <Grid container
                rowSpacing={2}
                columnSpacing={{ xs: 1, sm: 2, md: 3 }}
                sx={{
                    paddingLeft: 2,
                    paddingRight: 2
                }}
            >
                <Grid item xs={12}>
                    <TextField
                        type="text"
                        variant="standard"
                        id="name-input"
                        label="name"
                        value={name}
                        onChange={(e) => {
                            targetNode.setName(e.target.value);
                            setName(e.target.value);
                        }}
                        inputProps={{ style: { color: "#868686" } }}
                    />
                </Grid>
                <Grid item xs={12}>Transform
                </Grid>
                <Grid item xs={12}>
                    <Grid container
                        rowSpacing={2}
                        columnSpacing={{ xs: 1, sm: 2, md: 3 }}
                        sx={{
                            padding: 1,
                            color: "#868686"
                        }}
                    >
                        <Grid item xs={12}>Position
                        </Grid>
                        <Grid item xs={4}>
                            <TextField
                                type="number"
                                variant="standard"
                                id="position-x"
                                label="x"
                                value={position[0]}
                                onChange={(e) => {
                                    let tempPosition = targetNode.getPosition();
                                    if (Util.isValidNum(e.target.value)) {
                                        const floatValue = parseFloat(e.target.value);
                                        tempPosition.x = floatValue;
                                        targetNode.setPosition(tempPosition);

                                        setPosition([floatValue, position[1], position[2]]);
                                    } else {
                                        tempPosition.x = 0.0;
                                        targetNode.setPosition(tempPosition);
                                        setPosition([e.target.value, position[1], position[2]]);
                                    }
                                }}
                                inputProps={{ style: { color: "#868686" } }}
                            />
                        </Grid>
                        <Grid item xs={4}>
                            <TextField
                                type="number"
                                variant="standard"
                                id="position-y"
                                label="y"
                                value={position[1]}
                                onChange={(e) => {
                                    let tempPosition = targetNode.getPosition();
                                    if (Util.isValidNum(e.target.value)) {
                                        const floatValue = parseFloat(e.target.value);
                                        tempPosition.y = floatValue;
                                        targetNode.setPosition(tempPosition);

                                        setPosition([position[0], floatValue, position[2]]);
                                    } else {
                                        tempPosition.y = 0.0;
                                        targetNode.setPosition(tempPosition);
                                        setPosition([position[0], e.target.value, position[2]]);
                                    }
                                }}
                                inputProps={{ style: { color: "#868686" } }}
                            />
                        </Grid>
                        <Grid item xs={4}>
                            <TextField
                                type="number"
                                variant="standard"
                                id="position-z"
                                label="z"
                                value={position[2]}
                                onChange={(e) => {
                                    let tempPosition = targetNode.getPosition();
                                    if (Util.isValidNum(e.target.value)) {
                                        const floatValue = parseFloat(e.target.value);
                                        tempPosition.z = floatValue;
                                        targetNode.setPosition(tempPosition);

                                        setPosition([position[0], position[1], floatValue]);
                                    } else {
                                        tempPosition.z = 0.0;
                                        targetNode.setPosition(tempPosition);
                                        setPosition([position[0], position[1], e.target.value]);
                                    }
                                }}
                                inputProps={{ style: { color: "#868686" } }}
                            />
                        </Grid>
                        <Grid item xs={12}>Rotation
                        </Grid>
                        <Grid item xs={4}><TextField
                            type="number"
                            variant="standard"
                            id="rotation-x"
                            label="x"
                            value={rotation[0]}
                            onChange={(e) => {
                                if (Util.isValidNum(e.target.value)) {
                                    let tempRot = [parseFloat(e.target.value), rotation[1], rotation[2]];
                                    targetNode.setRotationByEuler(new module.vec3(tempRot[0], tempRot[1], tempRot[2]));
                                    setRotation(tempRot);
                                } else {
                                    setRotation([e.target.value, rotation[1], rotation[2]]);
                                }
                            }}
                            inputProps={{ style: { color: "#868686" } }}
                        />
                        </Grid>
                        <Grid item xs={4}>
                            <TextField
                                type="number"
                                variant="standard"
                                id="rotation-y"
                                label="y"
                                value={rotation[1]}
                                onChange={(e) => {
                                    if (Util.isValidNum(e.target.value)) {
                                        let tempRot = [rotation[0], parseFloat(e.target.value), rotation[2]];
                                        targetNode.setRotationByEuler(new module.vec3(tempRot[0], tempRot[1], tempRot[2]));
                                        setRotation(tempRot);
                                    } else {
                                        setRotation([rotation[0], e.target.value, rotation[2]]);
                                    }
                                }}
                                inputProps={{ style: { color: "#868686" } }}
                            />
                        </Grid>
                        <Grid item xs={4}>
                            <TextField
                                type="number"
                                variant="standard"
                                id="rotation-z"
                                label="z"
                                value={rotation[2]}
                                onChange={(e) => {
                                    if (Util.isValidNum(e.target.value)) {
                                        let tempRot = [rotation[0], rotation[1], parseFloat(e.target.value)];
                                        targetNode.setRotationByEuler(new module.vec3(tempRot[0], tempRot[1], tempRot[2]));
                                        setRotation(tempRot);
                                    } else {
                                        setRotation([rotation[0], rotation[1], e.target.value]);
                                    }
                                }}
                                inputProps={{ style: { color: "#868686" } }}
                            />
                        </Grid>
                        <Grid item xs={12}>Scale
                        </Grid>
                        <Grid item xs={4}>
                            <TextField
                                type="number"
                                variant="standard"
                                id="scale-x"
                                label="x"
                                value={scale[0]}
                                onChange={(e) => {
                                    let tempScale = targetNode.getScale();
                                    if (Util.isValidNum(e.target.value)) {
                                        const floatValue = parseFloat(e.target.value);
                                        tempScale.x = floatValue;
                                        targetNode.setScale(tempScale);

                                        setScale([floatValue, scale[1], scale[2]]);
                                    } else {
                                        tempScale.x = 0.0;
                                        targetNode.setScale(tempScale);
                                        setScale([e.target.value, position[1], position[2]]);
                                    }
                                }}
                                inputProps={{ style: { color: "#868686" } }}
                            />
                        </Grid>
                        <Grid item xs={4}>
                            <TextField
                                type="number"
                                variant="standard"
                                id="scale-y"
                                label="y"
                                value={scale[1]}
                                onChange={(e) => {
                                    let tempScale = targetNode.getScale();
                                    if (Util.isValidNum(e.target.value)) {
                                        const floatValue = parseFloat(e.target.value);
                                        tempScale.y = floatValue;
                                        targetNode.setScale(tempScale);

                                        setScale([scale[0], floatValue, scale[2]]);
                                    } else {
                                        tempScale.y = 0.0;
                                        targetNode.setScale(tempScale);
                                        setScale([scale[0], e.target.value, scale[2]]);
                                    }
                                }}
                                inputProps={{ style: { color: "#868686" } }}
                            />
                        </Grid>
                        <Grid item xs={4}>
                            <TextField
                                type="number"
                                variant="standard"
                                id="scale-z"
                                label="z"
                                value={scale[2]}
                                onChange={(e) => {
                                    let tempScale = targetNode.getScale();
                                    if (Util.isValidNum(e.target.value)) {
                                        const floatValue = parseFloat(e.target.value);
                                        tempScale.z = floatValue;
                                        targetNode.setScale(tempScale);

                                        setScale([scale[0], scale[2], floatValue]);
                                    } else {
                                        tempScale.z = 0.0;
                                        targetNode.setScale(tempScale);
                                        setScale([scale[0], scale[2], e.target.value]);
                                    }
                                }}
                                inputProps={{ style: { color: "#868686" } }}
                            />
                        </Grid>
                    </Grid>
                </Grid>

                <Grid item xs={12}>Camera</Grid>
                <Grid item xs={12}><CameraView targetNode={targetNode} /></Grid>
                <Grid item xs={12}>Light</Grid>
                <Grid item xs={12}><LightView targetNode={targetNode} /></Grid>
            </Grid>

        </Box>
    )
}

export default NodeView;