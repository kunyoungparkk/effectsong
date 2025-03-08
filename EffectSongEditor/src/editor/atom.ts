import { atom } from 'jotai';
import * as core from '../core/effectsong-core';
import { hierarchyNodeType } from './common';

export const selectedNodeAtom = atom<core.Node | null>(null);

export const hierarchyDataAtom = atom<Array<hierarchyNodeType>>([]);

export const notifyMessageAtom = atom({ open: false, success: false, message: '' });

export const loadingAtom = atom(false);