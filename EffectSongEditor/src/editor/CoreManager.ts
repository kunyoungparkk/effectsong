import createModule from "../core/effectsong-core.js"
import * as core from '../core/effectsong-core';
import AudioPlayer from "react-h5-audio-player";

export default class CoreManager {
    private static instance: CoreManager;
    private module!: core.MainModule;
    private renderer!: core.Renderer;
    private artShader!: core.ArtShader;

    private constructor() { }

    public static getInstance() {
        if (!this.instance) {
            this.instance = new CoreManager();
        }
        return this.instance;
    }

    public async initialize(canvasID: string, width: number, height: number, 
        audioRef:React.RefObject<AudioPlayer>): Promise<void> {
        await createModule().then((module: core.MainModule) => {
            (module as any)['canvas']! = document.getElementById(canvasID);
            //module.canvas.addEventListener("webglcontextlost", (e) => { alert('WebGL context lost. You will need to reload the page.'); e.preventDefault(); }, false);
        
            module.initialize(width, height,
                module.addFunction(() => audioRef.current!.audio.current!.currentTime, "f"),
                module.addFunction(() => audioRef.current!.isPlaying(), "i"));

            this.module = module;
            this.renderer = module.Renderer.getInstance()!;
            this.artShader = module.ArtShader.getInstance()!;

            //DefaultMusic
            const startMusicPath = module.getRootPath() + "res/music/unity.mp3";
            this.renderer.setAudioFile(startMusicPath);
            let arrayBuffer = module.FS.readFile(startMusicPath);
            const blob = new Blob([arrayBuffer], { type: "audio/mpeg" });
            const url = URL.createObjectURL(blob);
            audioRef.current!.audio.current!.src = url;
            module.FS.unlink(startMusicPath);

            //DefaultScene
            let scene = new module.Scene();
            scene.setName("DefaultScene");
            let camNode = new module.Node(scene, scene);
            camNode.setName("DefaultCamera");
            scene.addChild(camNode);
            let cam = new module.Camera(camNode);
            cam.fov = 90.0;
            camNode.setPosition(new module.vec3(0, 0, -5));
            camNode.setRotation(new module.quat(0, 0, 1, 0));
            camNode.setCamera(cam);
            this.renderer.addScene(scene);
            this.renderer.setActiveCamera(cam);
            
            return;
        })
    }
    public deInitialize() {
        this.module.deInitialize();
    }

    public getModule(): core.MainModule {
        return this.module;
    }
    public getRenderer(): core.Renderer {
        return this.renderer;
    }
    public getArtShader(): core.ArtShader {
        return this.artShader;
    }

}