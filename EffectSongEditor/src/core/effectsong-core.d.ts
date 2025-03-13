// TypeScript bindings for emscripten-generated code.  Automatically generated at compile time.
declare namespace RuntimeExports {
    /** @param {string=} sig */
    function addFunction(func: any, sig?: string | undefined): any;
    function removeFunction(index: any): void;
    namespace FS {
        export let root: any;
        export let mounts: any[];
        export let devices: {};
        export let streams: any[];
        export let nextInode: number;
        export let nameTable: any;
        export let currentPath: string;
        export let initialized: boolean;
        export let ignorePermissions: boolean;
        export let filesystems: any;
        export let syncFSRequests: number;
        export let readFiles: {};
        export { ErrnoError };
        export { FSStream };
        export { FSNode };
        export function lookupPath(path: any, opts?: {}): {
            path: string;
            node?: undefined;
        } | {
            path: string;
            node: any;
        };
        export function getPath(node: any): any;
        export function hashName(parentid: any, name: any): number;
        export function hashAddNode(node: any): void;
        export function hashRemoveNode(node: any): void;
        export function lookupNode(parent: any, name: any): any;
        export function createNode(parent: any, name: any, mode: any, rdev: any): any;
        export function destroyNode(node: any): void;
        export function isRoot(node: any): boolean;
        export function isMountpoint(node: any): boolean;
        export function isFile(mode: any): boolean;
        export function isDir(mode: any): boolean;
        export function isLink(mode: any): boolean;
        export function isChrdev(mode: any): boolean;
        export function isBlkdev(mode: any): boolean;
        export function isFIFO(mode: any): boolean;
        export function isSocket(mode: any): boolean;
        export function flagsToPermissionString(flag: any): string;
        export function nodePermissions(node: any, perms: any): 0 | 2;
        export function mayLookup(dir: any): any;
        export function mayCreate(dir: any, name: any): any;
        export function mayDelete(dir: any, name: any, isdir: any): any;
        export function mayOpen(node: any, flags: any): any;
        export function checkOpExists(op: any, err: any): any;
        export let MAX_OPEN_FDS: number;
        export function nextfd(): number;
        export function getStreamChecked(fd: any): any;
        export function getStream(fd: any): any;
        export function createStream(stream: any, fd?: number): any;
        export function closeStream(fd: any): void;
        export function dupStream(origStream: any, fd?: number): any;
        export function doSetAttr(stream: any, node: any, attr: any): void;
        export namespace chrdev_stream_ops {
            function open(stream: any): void;
            function llseek(): never;
        }
        export function major(dev: any): number;
        export function minor(dev: any): number;
        export function makedev(ma: any, mi: any): number;
        export function registerDevice(dev: any, ops: any): void;
        export function getDevice(dev: any): any;
        export function getMounts(mount: any): any[];
        export function syncfs(populate: any, callback: any): void;
        export function mount(type: any, opts: any, mountpoint: any): any;
        export function unmount(mountpoint: any): void;
        export function lookup(parent: any, name: any): any;
        export function mknod(path: any, mode: any, dev: any): any;
        export function statfs(path: any): any;
        export function statfsStream(stream: any): any;
        export function statfsNode(node: any): {
            bsize: number;
            frsize: number;
            blocks: number;
            bfree: number;
            bavail: number;
            files: any;
            ffree: number;
            fsid: number;
            flags: number;
            namelen: number;
        };
        export function create(path: any, mode?: number): any;
        export function mkdir(path: any, mode?: number): any;
        export function mkdirTree(path: any, mode: any): void;
        export function mkdev(path: any, mode: any, dev: any): any;
        export function symlink(oldpath: any, newpath: any): any;
        export function rename(old_path: any, new_path: any): void;
        export function rmdir(path: any): void;
        export function readdir(path: any): any;
        export function unlink(path: any): void;
        export function readlink(path: any): any;
        export function stat(path: any, dontFollow: any): any;
        export function fstat(fd: any): any;
        export function lstat(path: any): any;
        export function doChmod(stream: any, node: any, mode: any, dontFollow: any): void;
        export function chmod(path: any, mode: any, dontFollow: any): void;
        export function lchmod(path: any, mode: any): void;
        export function fchmod(fd: any, mode: any): void;
        export function doChown(stream: any, node: any, dontFollow: any): void;
        export function chown(path: any, uid: any, gid: any, dontFollow: any): void;
        export function lchown(path: any, uid: any, gid: any): void;
        export function fchown(fd: any, uid: any, gid: any): void;
        export function doTruncate(stream: any, node: any, len: any): void;
        export function truncate(path: any, len: any): void;
        export function ftruncate(fd: any, len: any): void;
        export function utime(path: any, atime: any, mtime: any): void;
        export function open(path: any, flags: any, mode?: number): any;
        export function close(stream: any): void;
        export function isClosed(stream: any): boolean;
        export function llseek(stream: any, offset: any, whence: any): any;
        export function read(stream: any, buffer: any, offset: any, length: any, position: any): any;
        export function write(stream: any, buffer: any, offset: any, length: any, position: any, canOwn: any): any;
        export function allocate(stream: any, offset: any, length: any): void;
        export function mmap(stream: any, length: any, position: any, prot: any, flags: any): any;
        export function msync(stream: any, buffer: any, offset: any, length: any, mmapFlags: any): any;
        export function ioctl(stream: any, cmd: any, arg: any): any;
        export function readFile(path: any, opts?: {}): any;
        export function writeFile(path: any, data: any, opts?: {}): void;
        export function cwd(): any;
        export function chdir(path: any): void;
        export function createDefaultDirectories(): void;
        export function createDefaultDevices(): void;
        export function createSpecialDirectories(): void;
        export function createStandardStreams(input: any, output: any, error: any): void;
        export function staticInit(): void;
        export function init(input: any, output: any, error: any): void;
        export function quit(): void;
        export function findObject(path: any, dontResolveLastLink: any): any;
        export function analyzePath(path: any, dontResolveLastLink: any): {
            isRoot: boolean;
            exists: boolean;
            error: number;
            name: any;
            path: any;
            object: any;
            parentExists: boolean;
            parentPath: any;
            parentObject: any;
        };
        export function createPath(parent: any, path: any, canRead: any, canWrite: any): any;
        export function createFile(parent: any, name: any, properties: any, canRead: any, canWrite: any): any;
        export function createDataFile(parent: any, name: any, data: any, canRead: any, canWrite: any, canOwn: any): void;
        export function createDevice(parent: any, name: any, input: any, output: any): any;
        export function forceLoadFile(obj: any): boolean;
        export function createLazyFile(parent: any, name: any, url: any, canRead: any, canWrite: any): any;
    }
    let HEAPF32: any;
    let HEAPF64: any;
    let HEAP_DATA_VIEW: any;
    let HEAP8: any;
    let HEAPU8: any;
    let HEAP16: any;
    let HEAPU16: any;
    let HEAP32: any;
    let HEAPU32: any;
    let HEAP64: any;
    let HEAPU64: any;
    let FS_createPath: any;
    function FS_createDataFile(parent: any, name: any, fileData: any, canRead: any, canWrite: any, canOwn: any): void;
    function FS_createPreloadedFile(parent: any, name: any, url: any, canRead: any, canWrite: any, onload: any, onerror: any, dontCreateFile: any, canOwn: any, preFinish: any): void;
    function FS_unlink(path: any): any;
    let FS_createLazyFile: any;
    let FS_createDevice: any;
    let addRunDependency: any;
    let removeRunDependency: any;
}
declare class ErrnoError {
    constructor(errno: any);
    name: string;
    errno: any;
}
declare class FSStream {
    shared: {};
    set object(val: any);
    get object(): any;
    node: any;
    get isRead(): boolean;
    get isWrite(): boolean;
    get isAppend(): number;
    set flags(val: any);
    get flags(): any;
    set position(val: any);
    get position(): any;
}
declare class FSNode {
    constructor(parent: any, name: any, mode: any, rdev: any);
    node_ops: {};
    stream_ops: {};
    readMode: number;
    writeMode: number;
    mounted: any;
    parent: any;
    mount: any;
    id: number;
    name: any;
    mode: any;
    rdev: any;
    atime: number;
    mtime: number;
    ctime: number;
    set read(val: boolean);
    get read(): boolean;
    set write(val: boolean);
    get write(): boolean;
    get isFolder(): any;
    get isDevice(): any;
}
interface WasmModule {
}

type EmbindString = ArrayBuffer|Uint8Array|Uint8ClampedArray|Int8Array|string;
export interface ClassHandle {
  isAliasOf(other: ClassHandle): boolean;
  delete(): void;
  deleteLater(): this;
  isDeleted(): boolean;
  clone(): this;
}
export interface Node extends ClassHandle {
  m_bAudioReactiveScale: boolean;
  m_reactiveOriginScale: number;
  m_reactiveChangingScale: number;
  getName(): string;
  setName(_0: EmbindString): void;
  addChild(_0: Node | null): void;
  removeChild(_0: Node | null): void;
  getParent(): Node | null;
  setParent(_0: Node | null): void;
  getChildAt(_0: number): Node | null;
  getChildByName(_0: EmbindString): Node | null;
  getChildrenCount(): number;
  getCamera(): Camera | null;
  setCamera(_0: Camera | null): void;
  getLight(): Light | null;
  setLight(_0: Light | null): void;
  getPosition(): vec3;
  getEulerRotation(): vec3;
  getScale(): vec3;
  getFront(): vec3;
  getLeft(): vec3;
  setPosition(_0: vec3): void;
  setRotationByEuler(_0: vec3): void;
  setScale(_0: vec3): void;
  getGlobalPosition(): vec3;
  getGlobalScale(): vec3;
  getGlobalFront(): vec3;
  getGlobalLeft(): vec3;
  getRotation(): quat;
  setRotation(_0: quat): void;
  getGlobalRotation(): quat;
}

export interface Scene extends Node {
  getCameraCount(): number;
  addCamera(_0: Camera | null): void;
  removeCamera(_0: Camera | null): void;
  getCameraAt(_0: number): Camera | null;
  addLight(_0: Light | null): void;
  removeLight(_0: Light | null): void;
}

export interface ProjectionTypeValue<T extends number> {
  value: T;
}
export type ProjectionType = ProjectionTypeValue<0>|ProjectionTypeValue<1>;

export interface Camera extends ClassHandle {
  projectionType: ProjectionType;
  aspectRatio: number;
  fov: number;
  xMag: number;
  yMag: number;
  zFar: number;
  zNear: number;
  get name(): string;
  set name(value: EmbindString);
  getNode(): Node | null;
}

export interface LightTypeValue<T extends number> {
  value: T;
}
export type LightType = LightTypeValue<0>|LightTypeValue<1>|LightTypeValue<2>|LightTypeValue<3>;

export interface Light extends ClassHandle {
  intensity: number;
  range: number;
  innerConeAngle: number;
  outerConeAngle: number;
  lightType: LightType;
  get name(): string;
  set name(value: EmbindString);
  color: vec3;
  getNode(): Node | null;
}

export interface Renderer extends ClassHandle {
  addScene(_0: Scene | null): void;
  removeScene(_0: Scene | null): void;
  getSceneAt(_0: number): Scene | null;
  getSceneByName(_0: EmbindString): Scene | null;
  getSceneCount(): number;
  setActiveCamera(_0: Camera | null): void;
  getActiveCamera(): Camera | null;
  getWidth(): number;
  getHeight(): number;
  resize(_0: number, _1: number): void;
  setAudioFile(_0: EmbindString): boolean;
  getDiffuseIBLIntensity(): number;
  setDiffuseIBLIntensity(_0: number): void;
  getCurrentEnergy(): number;
  getBackgroundColor(): vec4;
  setBackgroundColor(_0: vec4): void;
}

export interface ArtShader extends ClassHandle {
  getVertexShader(): string;
  setVertexShader(_0: EmbindString): boolean;
  getPrimitiveMode(): number;
  setPrimitiveMode(_0: number): void;
  getVertexCount(): number;
  setVertexCount(_0: number): void;
  getLastCompileError(): string;
}

export interface vec2 extends ClassHandle {
  x: number;
  y: number;
}

export interface vec3 extends ClassHandle {
  x: number;
  y: number;
  z: number;
}

export interface vec4 extends ClassHandle {
  x: number;
  y: number;
  z: number;
  w: number;
}

export interface quat extends ClassHandle {
  x: number;
  y: number;
  z: number;
  w: number;
}

interface EmbindModule {
  initialize(_0: number, _1: number, _2: number, _3: number): void;
  deInitialize(): void;
  loadGLTFData(_0: EmbindString): boolean;
  getRootPath(): string;
  Node: {
    new(_0: Scene | null, _1: Node | null): Node;
  };
  Scene: {
    new(): Scene;
  };
  ProjectionType: {PERSPECTIVE: ProjectionTypeValue<0>, ORTHOGRAPHIC: ProjectionTypeValue<1>};
  Camera: {
    new(_0: Node | null): Camera;
  };
  LightType: {NONE: LightTypeValue<0>, DIRECTIONAL_LIGHT: LightTypeValue<1>, POINT_LIGHT: LightTypeValue<2>, SPOT_LIGHT: LightTypeValue<3>};
  Light: {
    new(_0: Node | null): Light;
  };
  Renderer: {
    getInstance(): Renderer | null;
  };
  ArtShader: {
    getInstance(): ArtShader | null;
  };
  vec2: {
    new(_0: number, _1: number): vec2;
  };
  vec3: {
    new(_0: number, _1: number, _2: number): vec3;
  };
  vec4: {
    new(_0: number, _1: number, _2: number, _3: number): vec4;
  };
  quat: {
    new(_0: number, _1: number, _2: number, _3: number): quat;
  };
}

export type MainModule = WasmModule & typeof RuntimeExports & EmbindModule;
export default function MainModuleFactory (options?: unknown): Promise<MainModule>;
