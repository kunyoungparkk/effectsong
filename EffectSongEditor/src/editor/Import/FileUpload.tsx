import React, { useState } from "react";
import { Box, Typography } from "@mui/material";
import CloudUploadIcon from "@mui/icons-material/CloudUpload";

const FileUpload = ({
  hoverLabel = "Click or drag to upload files",
  dropLabel = "Drop file here",
  backgroundColor = "#fff",
  width,
  height,
  accept,
  onChange,
  onDrop,
  isMultiple = false,
}: any) => {
  const [labelText, setLabelText] = useState(hoverLabel);
  const [isDragOver, setIsDragOver] = useState(false);
  const [isMouseOver, setIsMouseOver] = useState(false);

  const stopDefaults = (e: any) => {
    e.stopPropagation();
    e.preventDefault();
  };

  const dragEvents = {
    onMouseEnter: () => {
      setIsMouseOver(true);
    },
    onMouseLeave: () => {
      setIsMouseOver(false);
    },
    onDragEnter: (e: any) => {
      stopDefaults(e);
      setIsDragOver(true);
      setLabelText(dropLabel);
    },
    onDragLeave: (e: any) => {
      stopDefaults(e);
      setIsDragOver(false);
      setLabelText(hoverLabel);
    },
    onDragOver: stopDefaults,
    onDrop: (e: any) => {
      stopDefaults(e);
      setLabelText(hoverLabel);
      setIsDragOver(false);
      const files = e.dataTransfer?.files;
      if (files) {
        onDrop(e);
      } else {
        setLabelText("upload error. try again.");
        return;
      }
    },
  };

  const onChangeProc = (e: any) => {
    const files = e.target?.files;
    if (files) {
      setLabelText(files[0].name);
    }
    onChange(e);
  };
  return (
    <>
      <input
        onChange={onChangeProc}
        accept={accept}
        id="file-upload"
        type="file"
        style={{
          display: "none",
        }}
        multiple={isMultiple}
      />

      <label
        htmlFor="file-upload"
        {...dragEvents}
        style={
          isDragOver
            ? {
              "& img": {
                opacity: 0.3,
              },
              "& p, svg": {
                opacity: 1,
              },
              width: { width },
              height: { height },
            } as unknown as React.CSSProperties
            : {
              cursor: "pointer",
              textAlign: "center",
              "&:hover p,&:hover svg,& img": {
                opacity: 1,
              },
              "& p, svg": {
                opacity: 0.4,
              },
              "&:hover img": {
                opacity: 0.3,
              },
              width: { width },
              height: { height },
            } as unknown as React.CSSProperties
        }
      >
        <Box
          width={width}
          height={height}
          bgcolor={backgroundColor}
          sx={{ border: 1 }}
          style={{
            pointerEvents: "none",
          }}
        >
          {(true || isDragOver || isMouseOver) && (
            <>
              <Box
                height={height}
                width={width}
                style={{
                  display: "flex",
                  justifyContent: "center",
                  flexDirection: "column",
                  alignItems: "center",
                  position: "absolute",
                }}
              >
                <CloudUploadIcon fontSize="large" />
                <Typography>{labelText}</Typography>
              </Box>
            </>
          )}
        </Box>
      </label>
    </>
  );
};

export default FileUpload;
