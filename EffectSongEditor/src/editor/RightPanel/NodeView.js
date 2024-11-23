import { useEffect, useState } from 'react'
import { rotationMatrix, multiply } from 'mathjs';
import { Box, TextField, Grid, InputLabel, FormControl } from '@mui/material';
import useUtil from '../Util';

const NodeView = ({ module, targetNode }) => {
    const Util = useUtil();
    const [position, setPosition] = useState([0.0, 0.0, 0.0]);
    const [rotation, setRotation] = useState([0.0, 0.0, 0.0]);
    const [scale, setScale] = useState([0.0, 0.0, 0.0]);

    useEffect(() => {
        if (!module || !targetNode) {
            return;
        }
        const inputPos = targetNode.getPosition();
        const inputRot = targetNode.getRotation();
        const inputEuler = Util.eulerFromQuat(inputRot.w, inputRot.x, inputRot.y, inputRot.z);
        const inputScale = targetNode.getScale();

        setPosition([inputPos.x, inputPos.y, inputPos.z]);
        setRotation(inputEuler);
        setScale([inputScale.x, inputScale.y, inputScale.z]);
    }, [targetNode])
    return (
        <Box>
            <Grid container
                rowSpacing={2}
                columnSpacing={{ xs: 1, sm: 2, md: 3 }}
                sx={{
                    padding: 2
                }}
            >
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
                            <FormControl variant="standard">
                                <InputLabel htmlFor="position-x">x</InputLabel>
                                <TextField
                                    type="number"
                                    variant="standard"
                                    id="position-x"
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
                                    color="secondary"
                                />
                            </FormControl>
                        </Grid>
                        <Grid item xs={4}>
                            <FormControl variant="standard">
                                <InputLabel htmlFor="position-y">y</InputLabel>
                                <TextField
                                    type="number"
                                    variant="standard"
                                    id="position-y"
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
                                    color="secondary"
                                />
                            </FormControl>
                        </Grid>
                        <Grid item xs={4}>
                            <FormControl variant="standard">
                                <InputLabel htmlFor="position-z">z</InputLabel>
                                <TextField
                                    type="number"
                                    variant="standard"
                                    id="position-z"
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
                                    color="secondary"
                                />
                            </FormControl>
                        </Grid>
                        <Grid item xs={12}>Rotation
                        </Grid>
                        <Grid item xs={4}>
                            <FormControl variant="standard">
                                <InputLabel htmlFor="rotation-x">x</InputLabel>
                                <TextField
                                    type="number"
                                    variant="standard"
                                    id="rotation-x"
                                    value={rotation[0]}
                                    onChange={(e) => {
                                        if (Util.isValidNum(e.target.value)) {
                                            let tempRot = [parseFloat(e.target.value), rotation[1], rotation[2]];
                                            const tempQuat = Util.quatFromEuler(tempRot[0], tempRot[1], tempRot[2]);
                                            targetNode.setRotation(new module.quat(tempQuat.w, tempQuat.x, tempQuat.y, tempQuat.z));
                                            setRotation(tempRot);
                                        } else {
                                            let tempRot = [0.0, rotation[1], rotation[2]];
                                            const tempQuat = Util.quatFromEuler(tempRot[0], tempRot[1], tempRot[2]);
                                            targetNode.setRotation(new module.quat(tempQuat.w, tempQuat.x, tempQuat.y, tempQuat.z));
                                            tempRot[0] = e.target.value;
                                            setRotation(tempRot);
                                        }
                                    }}
                                    inputProps={{ style: { color: "#868686" } }}
                                    color="secondary"
                                />
                            </FormControl>
                        </Grid>
                        <Grid item xs={4}>
                            <FormControl variant="standard">
                                <InputLabel htmlFor="rotation-y">y</InputLabel>
                                <TextField
                                    type="number"
                                    variant="standard"
                                    id="rotation-y"
                                    value={rotation[1]}
                                    onChange={(e) => {
                                        let tempRot = [];
                                        if (Util.isValidNum(e.target.value)) {
                                            tempRot = [rotation[0], parseFloat(e.target.value), rotation[2]];
                                        } else {
                                            tempRot = [rotation[0], 0.0, rotation[2]];
                                        }
                                        const tempQuat = Util.quatFromEuler(tempRot[0], tempRot[1], tempRot[2]);
                                        targetNode.setRotation(new module.quat(tempQuat.w, tempQuat.x, tempQuat.y, tempQuat.z));
                                        setRotation(tempRot);
                                    }}
                                    inputProps={{ style: { color: "#868686" } }}
                                    color="secondary"
                                />
                            </FormControl>
                        </Grid>
                        <Grid item xs={4}>
                            <FormControl variant="standard">
                                <InputLabel htmlFor="rotation-z">z</InputLabel>
                                <TextField
                                    type="number"
                                    variant="standard"
                                    id="rotation-z"
                                    value={rotation[2]}
                                    onChange={(e) => {
                                        let tempRot = [];
                                        if (Util.isValidNum(e.target.value)) {
                                            tempRot = [rotation[0], rotation[1], parseFloat(e.target.value)];
                                        } else {
                                            tempRot = [rotation[0], rotation[1], 0.0];
                                        }
                                        const tempQuat = Util.quatFromEuler(tempRot[0], tempRot[1], tempRot[2]);
                                        targetNode.setRotation(new module.quat(tempQuat.w, tempQuat.x, tempQuat.y, tempQuat.z));
                                        setRotation(tempRot);
                                    }}
                                    inputProps={{ style: { color: "#868686" } }}
                                    color="secondary"
                                />
                            </FormControl>
                        </Grid>
                        <Grid item xs={12}>Scale
                        </Grid>
                        <Grid item xs={4}>
                            <FormControl variant="standard">
                                <InputLabel htmlFor="scale-x">x</InputLabel>
                                <TextField
                                    type="number"
                                    variant="standard"
                                    id="scale-x"
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
                                    color="secondary"
                                />
                            </FormControl>
                        </Grid>
                        <Grid item xs={4}>
                            <FormControl variant="standard">
                                <InputLabel htmlFor="scale-y">y</InputLabel>
                                <TextField
                                    type="number"
                                    variant="standard"
                                    id="scale-y"
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
                                    color="secondary"
                                />
                            </FormControl>
                        </Grid>
                        <Grid item xs={4}>
                            <FormControl variant="standard">
                                <InputLabel htmlFor="scale-z">z</InputLabel>
                                <TextField
                                    type="number"
                                    variant="standard"
                                    id="scale-z"
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
                                    color="secondary"
                                />
                            </FormControl>
                        </Grid>
                    </Grid>
                </Grid>
            </Grid>

        </Box>
    )
}

export default NodeView;