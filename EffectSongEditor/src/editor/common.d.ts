//Hierarchy Data State
export type hierarchyNodeType = {
    id: string,
    name: string,
    isSelected: boolean,
    children: Array<hierarchyNodeType>
}