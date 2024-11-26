import { useState } from "react";
import { Button, Modal, Box } from "@mui/material";
import CloudUploadIcon from "@mui/icons-material/CloudUpload";
import FileUpload from "./FileUpload";

const GLTFImport = ({ module, updateHierarchy }) => {
  const [modalOpen, setModalOpen] = useState(false);

  const procGLTFInput = (files) => {
    let reader = new FileReader();

    let readFileCount = 0;
    let gltfFilePath = "";
    const GLTF_ROOT_PATH = module.getRootPath() + "res/gltf/";
    
    if (!module.FS.analyzePath(GLTF_ROOT_PATH).exists) {
        module.FS.mkdir(GLTF_ROOT_PATH);
    }

    for (let i = 0; i < files.length; i++) {
      let file = files[i];
      reader = new FileReader();
      reader.onload = function (e) {
        let arrayBuffer = e.target.result;
        let filePath = GLTF_ROOT_PATH + file.name;
        if (file.name.split(".")[1] === "gltf") {
          gltfFilePath = filePath;
        }
        module.FS.writeFile(filePath, new Uint8Array(arrayBuffer));
        readFileCount++;
        if (readFileCount === files.length) {
          module.loadGLTFData(gltfFilePath);

          const dir_info = module.FS.readdir(GLTF_ROOT_PATH);
          for (let i = 2; i < dir_info.length; i++) {
            module.FS.unlink(GLTF_ROOT_PATH + dir_info[i]);
          }
          updateHierarchy();
          setModalOpen(false);
        }
      };
      reader.readAsArrayBuffer(file);
    }
  };

  return (
    <div>
      <Button
        size="small"
        variant="contained"
        startIcon={<CloudUploadIcon />}
        sx={{ width: "100%" }}
        onClick={() => {
          setModalOpen(true);
        }}
      >
        gltf
      </Button>
      <Modal
        open={modalOpen}
        onClose={() => {
          setModalOpen(false);
        }}
        aria-labelledby="gltf-import-modal"
        aria-describedby="gltf-import-description"
      >
        <Box
          sx={{
            position: "absolute",
            top: "50%",
            left: "50%",
            transform: "translate(-50%, -50%)",
            width: "50%",
            height: "50%",
            bgcolor: "background.paper",
            border: "2px solid #000",
            boxShadow: 24,
            p: 4,
          }}
        >
          <FileUpload
            width="100%"
            height="100%"
            isMultiple={true}
            onChange={(e) => {
              procGLTFInput(e.target?.files);
            }}
            onDrop={(e) => {
              let files = e.dataTransfer?.files;
              procGLTFInput(files);
            }}
            hoverLabel="Click or drag to upload files"
            dropLabel="Drop files here"
          />
        </Box>
      </Modal>
    </div>
  );
};
export default GLTFImport;
