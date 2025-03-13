import { useEffect, useState } from "react";
import {
  Box,
  TextField,
  Grid,
  IconButton,
  Divider,
  Button,
} from "@mui/material";
import Util from "../Util";
import CameraView from "./CameraView";
import LightView from "./LightView";
import MusicNoteIcon from "@mui/icons-material/MusicNote";
import MusicOffIcon from "@mui/icons-material/MusicOff";
import DeleteForeverIcon from '@mui/icons-material/DeleteForever';
import * as core from '../../core/effectsong-core'
import CoreManager from "../CoreManager";

import { useAtom } from 'jotai';
import { selectedNodeAtom } from "../atom";

type NodeViewType = {
  updateHierarchy: () => void
}
const NodeView = ({
  updateHierarchy
}: NodeViewType) => {
  const module = CoreManager.getInstance().getModule();

  const [selectedNode, setSelectedNode] = useAtom(selectedNodeAtom);
  if(!selectedNode){
    return<></>
  }

  const [name, setName] = useState("");
  const [position, setPosition] = useState<[string, string, string]>(['0', '0', '0']);
  const [rotation, setRotation] = useState<[string, string, string]>(['0', '0', '0']);
  const [scale, setScale] = useState<[string, string, string]>(['0', '0', '0']);

  const [audioReactiveScale, setAudioReactiveScale] = useState<boolean>(false);
  const [reactiveOriginScale, setReactiveOriginScale] = useState<string>('0');
  const [reactiveChangingScale, setReactiveChangingScale] = useState<string>('0');

  useEffect(() => {
    if (!selectedNode) {
      return;
    }
    const inputPos = selectedNode.getPosition();
    const inputEuler = selectedNode.getEulerRotation();
    const inputScale = selectedNode.getScale();

    setName(selectedNode.getName());
    setPosition([
      Util.roundToNearestStep(inputPos.x).toString(),
      Util.roundToNearestStep(inputPos.y).toString(),
      Util.roundToNearestStep(inputPos.z).toString(),
    ]);
    setRotation([
      Util.roundToNearestStep(inputEuler.x).toString(),
      Util.roundToNearestStep(inputEuler.y).toString(),
      Util.roundToNearestStep(inputEuler.z).toString(),
    ]);
    setScale([
      Util.roundToNearestStep(inputScale.x).toString(),
      Util.roundToNearestStep(inputScale.y).toString(),
      Util.roundToNearestStep(inputScale.z).toString(),
    ]);

    setAudioReactiveScale(selectedNode.m_bAudioReactiveScale);
    setReactiveOriginScale(
      Util.roundToNearestStep(selectedNode.m_reactiveOriginScale).toString()
    );
    setReactiveChangingScale(
      Util.roundToNearestStep(selectedNode.m_reactiveChangingScale).toString()
    );
  }, [selectedNode]);

  const removeNode = (node: core.Node) => {
    const parentNode = node.getParent();
    if (parentNode) {
      parentNode.removeChild(node);
    } else {
      CoreManager.getInstance().getRenderer().removeScene(node as core.Scene);
    }

    setSelectedNode(null);
    updateHierarchy();
  }

  const changePosition = (targetPosition: [string, string, string]) => {
    let tempPosition: [number, number, number] = [
      Util.isValidNum(targetPosition[0]) ? parseFloat(targetPosition[0]) : 0.0,
      Util.isValidNum(targetPosition[1]) ? parseFloat(targetPosition[1]) : 0.0,
      Util.isValidNum(targetPosition[2]) ? parseFloat(targetPosition[2]) : 0.0
    ];

    selectedNode.setPosition(new module.vec3(...tempPosition));
    setPosition(targetPosition);
  };
  const changeRotation = (targetRotation: [string, string, string]) => {
    let tempRot: [number, number, number] = [
      Util.isValidNum(targetRotation[0]) ? parseFloat(targetRotation[0]) : 0.0,
      Util.isValidNum(targetRotation[1]) ? parseFloat(targetRotation[1]) : 0.0,
      Util.isValidNum(targetRotation[2]) ? parseFloat(targetRotation[2]) : 0.0
    ];

    selectedNode.setRotationByEuler(new module.vec3(...tempRot));
    setRotation(targetRotation);
  }
  const changeScale = (targetScale: [string, string, string]) => {
    let tempScale: [number, number, number] = [
      Util.isValidNum(targetScale[0]) ? parseFloat(targetScale[0]) : 0.0,
      Util.isValidNum(targetScale[1]) ? parseFloat(targetScale[1]) : 0.0,
      Util.isValidNum(targetScale[2]) ? parseFloat(targetScale[2]) : 0.0
    ];

    selectedNode.setScale(new module.vec3(...tempScale));
    setScale(targetScale);
  };

  return (
    <Box
      sx={{
        maxHeight: "calc(100vh - 110px)",
        overflowY: "auto",
        scrollbarWidth: "thin",
        scrollbarColor: "#868686 #2e2e2e",
      }}
    >
      <Grid
        container
        rowSpacing={2}
        columnSpacing={{ xs: 1, sm: 2, md: 3 }}
        sx={{
          paddingLeft: 2,
          paddingRight: 2,
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
              selectedNode.setName(e.target.value);
              setName(e.target.value);
              updateHierarchy();
            }}
            InputLabelProps={{
              shrink: true,
              style: { color: "#868686" },
            }}
            inputProps={{ style: { color: "white" } }}
            sx={{ width: "100%" }}
          />
        </Grid>
        <Grid item xs={12}>
          <Divider
            sx={{
              color: "#868686",
              "&::before, &::after": {
                borderColor: "#868686",
              },
            }}
          >
            Transform
          </Divider>
        </Grid>
        <Grid item xs={12}>
          <Grid
            container
            rowSpacing={2}
            columnSpacing={{ xs: 1, sm: 2, md: 3 }}
            sx={{
              padding: 1,
              color: "#868686",
            }}
          >
            <Grid item xs={12}>
              Position
            </Grid>
            <Grid item xs={4}>
              <TextField
                type="number"
                variant="standard"
                id="position-x"
                label="x"
                value={position[0]}
                onChange={(e: React.ChangeEvent<HTMLInputElement | HTMLTextAreaElement>) => {
                  changePosition([e.target.value, position[1], position[2]]);

                }}
                InputLabelProps={{
                  shrink: true,
                  style: { color: "#868686" },
                }}
                inputProps={{ style: { color: "white" } }}
              />
            </Grid>
            <Grid item xs={4}>
              <TextField
                type="number"
                variant="standard"
                id="position-y"
                label="y"
                value={position[1]}
                onChange={(e: React.ChangeEvent<HTMLInputElement | HTMLTextAreaElement>) => {
                  changePosition([position[0], e.target.value, position[2]]);
                }}
                InputLabelProps={{
                  shrink: true,
                  style: { color: "#868686" },
                }}
                inputProps={{ style: { color: "white" } }}
              />
            </Grid>
            <Grid item xs={4}>
              <TextField
                type="number"
                variant="standard"
                id="position-z"
                label="z"
                value={position[2]}
                onChange={(e: React.ChangeEvent<HTMLInputElement | HTMLTextAreaElement>) => {
                  changePosition([position[0], position[1], e.target.value]);
                }}
                InputLabelProps={{
                  shrink: true,
                  style: { color: "#868686" },
                }}
                inputProps={{ style: { color: "white" } }}
              />
            </Grid>
            <Grid item xs={12}>
              Rotation
            </Grid>
            <Grid item xs={4}>
              <TextField
                type="number"
                variant="standard"
                id="rotation-x"
                label="x"
                value={rotation[0]}
                onChange={(e: React.ChangeEvent<HTMLInputElement | HTMLTextAreaElement>) => {
                  changeRotation([e.target.value, rotation[1], rotation[2]]);
                }}
                InputLabelProps={{
                  shrink: true,
                  style: { color: "#868686" },
                }}
                inputProps={{ style: { color: "white" } }}
              />
            </Grid>
            <Grid item xs={4}>
              <TextField
                type="number"
                variant="standard"
                id="rotation-y"
                label="y"
                value={rotation[1]}
                onChange={(e: React.ChangeEvent<HTMLInputElement | HTMLTextAreaElement>) => {
                  changeRotation([rotation[0], e.target.value, rotation[2]]);
                }}
                InputLabelProps={{
                  shrink: true,
                  style: { color: "#868686" },
                }}
                inputProps={{ style: { color: "white" } }}
              />
            </Grid>
            <Grid item xs={4}>
              <TextField
                type="number"
                variant="standard"
                id="rotation-z"
                label="z"
                value={rotation[2]}
                onChange={(e: React.ChangeEvent<HTMLInputElement | HTMLTextAreaElement>) => {
                  changeRotation([rotation[0], rotation[1], e.target.value]);
                }}
                InputLabelProps={{
                  shrink: true,
                  style: { color: "#868686" },
                }}
                inputProps={{ style: { color: "white" } }}
              />
            </Grid>
            {/*Audio Reactive Scale*/}
            <Grid item xs={1}>
              <IconButton
                aria-label="use-reactive-scale"
                onClick={() => {
                  selectedNode.m_bAudioReactiveScale = !audioReactiveScale;
                  //when disable reactivescale
                  if (audioReactiveScale) {
                    const inputScale = selectedNode.getScale();
                    setScale([
                      Util.roundToNearestStep(inputScale.x).toString(),
                      Util.roundToNearestStep(inputScale.y).toString(),
                      Util.roundToNearestStep(inputScale.z).toString(),
                    ]);
                  }
                  setAudioReactiveScale(!audioReactiveScale);
                }}
                style={{ color: "#868686", padding: 0 }}
              >
                {audioReactiveScale ? (
                  <MusicNoteIcon sx={{ color: "white" }} />
                ) : (
                  <MusicOffIcon sx={{ color: "white" }} />
                )}
              </IconButton>
            </Grid>
            <Grid item xs={11}>
              Scale
            </Grid>
            {audioReactiveScale ? (
              <>
                <Grid item xs={6}>
                  <TextField
                    type="number"
                    variant="standard"
                    id="reactive-origin-scale"
                    label="origin scale"
                    value={reactiveOriginScale}
                    onChange={(e: React.ChangeEvent<HTMLInputElement | HTMLTextAreaElement>) => {
                      if (Util.isValidNum(e.target.value)) {
                        const floatValue = parseFloat(e.target.value);
                        selectedNode.m_reactiveOriginScale = floatValue;
                      }
                      setReactiveOriginScale(e.target.value);
                    }}
                    InputLabelProps={{
                      shrink: true,
                      style: { color: "#868686" },
                    }}
                    inputProps={{ style: { color: "white" } }}
                  />
                </Grid>
                <Grid item xs={6}>
                  <TextField
                    type="number"
                    variant="standard"
                    id="reactive-changing-scale"
                    label="reactive scale"
                    value={reactiveChangingScale}
                    onChange={(e: React.ChangeEvent<HTMLInputElement | HTMLTextAreaElement>) => {
                      if (Util.isValidNum(e.target.value)) {
                        const floatValue = parseFloat(e.target.value);
                        selectedNode.m_reactiveChangingScale = floatValue;
                      }
                      setReactiveChangingScale(e.target.value);
                    }}
                    InputLabelProps={{
                      shrink: true,
                      style: { color: "#868686" },
                    }}
                    inputProps={{ style: { color: "white" } }}
                  />
                </Grid>
              </>
            ) : (
              <>
                <Grid item xs={4}>
                  <TextField
                    type="number"
                    variant="standard"
                    id="scale-x"
                    label="x"
                    value={scale[0]}
                    onChange={(e: React.ChangeEvent<HTMLInputElement | HTMLTextAreaElement>) => {
                      changeScale([e.target.value, scale[1], scale[2]]);
                    }}
                    InputLabelProps={{
                      shrink: true,
                      style: { color: "#868686" },
                    }}
                    inputProps={{ style: { color: "white" } }}
                  />
                </Grid>
                <Grid item xs={4}>
                  <TextField
                    type="number"
                    variant="standard"
                    id="scale-y"
                    label="y"
                    value={scale[1]}
                    onChange={(e: React.ChangeEvent<HTMLInputElement | HTMLTextAreaElement>) => {
                      changeScale([scale[0], e.target.value, scale[2]]);
                    }}
                    InputLabelProps={{
                      shrink: true,
                      style: { color: "#868686" },
                    }}
                    inputProps={{ style: { color: "white" } }}
                  />
                </Grid>
                <Grid item xs={4}>
                  <TextField
                    type="number"
                    variant="standard"
                    id="scale-z"
                    label="z"
                    value={scale[2]}
                    onChange={(e: React.ChangeEvent<HTMLInputElement | HTMLTextAreaElement>) => {
                      changeScale([scale[0], scale[1], e.target.value]);
                    }}
                    InputLabelProps={{
                      shrink: true,
                      style: { color: "#868686" },
                    }}
                    inputProps={{ style: { color: "white" } }}
                  />
                </Grid>
              </>
            )}
          </Grid>
        </Grid>

        <Grid item xs={12}>
          <Divider
            sx={{
              color: "#868686",
              "&::before, &::after": {
                borderColor: "#868686",
              },
            }}
          >
            Camera
          </Divider>
        </Grid>
        <Grid item xs={12}>
          <CameraView targetNode={selectedNode} />
        </Grid>
        <Grid item xs={12}>
          <Divider
            sx={{
              color: "#868686",
              "&::before, &::after": {
                borderColor: "#868686",
              },
            }}
          >
            Light
          </Divider>
        </Grid>
        <Grid item xs={12}>
          <LightView targetNode={selectedNode} />
        </Grid>

        <Grid item xs={12}>
          <Divider
            sx={{
              color: "#868686",
              "&::before, &::after": {
                borderColor: "#868686",
              },
            }}
          >
            Control
          </Divider>
        </Grid>
        <Grid item xs={6}></Grid>
        <Grid item xs={6}>
          <Button
            size="small"
            variant="contained"
            startIcon={<DeleteForeverIcon />}
            sx={{ width: "100%", backgroundColor: "#7d0e0e" }}
            onClick={() => {
              removeNode(selectedNode);
            }}
          >
            remove
          </Button>
        </Grid>
      </Grid>
    </Box>
  );
};

export default NodeView;
