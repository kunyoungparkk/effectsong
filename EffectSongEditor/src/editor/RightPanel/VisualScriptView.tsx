import { Box, Grid } from "@mui/material";

const VisualScriptView = () => {
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
          Coming Soon . . .
        </Grid>
      </Grid>
    </Box>
  );
};

export default VisualScriptView;
