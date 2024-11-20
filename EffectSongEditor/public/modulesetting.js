var EFFECTSONG_CORE = null;
window.initializeModule = (onEngineInitialized) =>{
    createModule().then((instance)=>{
        //외부에서 모듈 접근을 위해
        EFFECTSONG_CORE = instance;
        onEngineInitialized();
    });
}