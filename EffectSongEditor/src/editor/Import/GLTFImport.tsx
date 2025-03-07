import { useState } from "react";
import { Button, Modal, Box } from "@mui/material";
import CloudUploadIcon from "@mui/icons-material/CloudUpload";
import FileUpload from "./FileUpload";
import * as core from '../../core/effectsong-core';

type gltfImportType = {
  module: core.MainModule,
  updateHierarchy: () => void,
  notify: (message: string, isSuccess: boolean) => void,
  setLoading: React.Dispatch<React.SetStateAction<boolean>>
}
const GLTFImport = ({ module, updateHierarchy, notify, setLoading }: gltfImportType) => {
  const [modalOpen, setModalOpen] = useState(false);

  const procGLTFInput = (files: FileList | null) => {
    if(!files){
      return;
    }

    //glb인지, gltf인지, 혹은 둘다, 혹은 없는지, 혹은 여러개인지 확인.
    let isGLTF = false;
    let isGLB = false;
    let modelCount = 0;
    let fileName = "";
    for (let i = 0; i < files.length; i++) {
      let splittedName = files[i].name.split(".");
      const ext = splittedName[1];
      if (ext && ext === 'gltf') {
        isGLTF = true;
        fileName = splittedName[0];
        modelCount++;
      }
      else if (ext && ext === 'glb') {
        isGLB = true;
        fileName = splittedName[0];
        modelCount++;
      }
    }

    if (!isGLTF && !isGLB) {
      notify('failed to load model.', false);
      return;
    } else if (modelCount > 1) {
      notify('can\'t import multiple models.', false);
      return;
    }

    setLoading(true);
    setModalOpen(false);

    const targetEXT = isGLTF ? "gltf" : "glb";
    const TARGET_GLTF_ROOT_PATH = module.getRootPath() + "res/" + fileName + "/";
    console.log(TARGET_GLTF_ROOT_PATH);
    if (!module.FS.analyzePath(TARGET_GLTF_ROOT_PATH, false).exists) {
      module.FS.mkdir(TARGET_GLTF_ROOT_PATH);
    }

    let gltfFilePath = "";
    let readPromises = [];
    for (let i = 0; i < files.length; i++) {
      readPromises.push(new Promise<void>((resolve, reject) => {
        let file = files[i];
        let reader = new FileReader();
        reader.onload = function (e: ProgressEvent<FileReader>) {
          if(!e.target){
            return false;
          }

          let arrayBuffer = e.target.result as ArrayBuffer;
          let filePath = TARGET_GLTF_ROOT_PATH + file.name;

          if (file.name.split(".")[1] === targetEXT) {
            gltfFilePath = filePath;
          }

          module.FS.writeFile(filePath, new Uint8Array(arrayBuffer));
          resolve();
        };
        reader.onerror = function (e) {
          reject(e);
        }
        reader.readAsArrayBuffer(file);
      }))
    }

    Promise.all(readPromises)
      .then(() => {
        module.loadGLTFData(gltfFilePath);

        //delete files after load
        const dir_info = module.FS.readdir(TARGET_GLTF_ROOT_PATH);
        for (let i = 2; i < dir_info.length; i++) {
          module.FS.unlink(TARGET_GLTF_ROOT_PATH + dir_info[i]);
        }
        module.FS.rmdir(TARGET_GLTF_ROOT_PATH);

        updateHierarchy();
        setLoading(false);
        notify(fileName + " loaded successfully ", true);
      })
      .catch((error) => {
        //delete loaded files
        const dir_info = module.FS.readdir(TARGET_GLTF_ROOT_PATH);
        for (let i = 2; i < dir_info.length; i++) {
          module.FS.unlink(TARGET_GLTF_ROOT_PATH + dir_info[i]);
        }
        module.FS.rmdir(TARGET_GLTF_ROOT_PATH);

        setLoading(false);
        notify(fileName + " loaded failed ", false);
        console.log(error);
      });
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
        gltf/glb
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
            onChange={(e: React.ChangeEvent<HTMLInputElement>) => {
              procGLTFInput(e.target.files);
            }}
            onDrop={(e: React.DragEvent<HTMLLabelElement>) => {
              let files = e.dataTransfer?.files;
              procGLTFInput(files);
            }}
            hoverLabel="Click or drag to upload all files (supports gltf with textures, embedded gltf, glb)"
            dropLabel="Drop files here"
          />
        </Box>
      </Modal>
    </div>
  );
};
export default GLTFImport;
