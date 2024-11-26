import { useEffect, useState } from "react";
import { Box, TextField, Grid } from "@mui/material";
const NodeView = ({ module, targetScene, updateHierarchy }) => {
  const [name, setName] = useState("");

  useEffect(() => {
    if (!module || !targetScene) {
      return;
    }
    setName(targetScene.getName());
  }, [targetScene]);
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
              targetScene.setName(e.target.value);
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
      </Grid>
    </Box>
  );
};

export default NodeView;
