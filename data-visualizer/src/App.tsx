import Editor from "./Editor"
import { useState, useCallback } from "react"
import { useDropzone } from 'react-dropzone'


export type Bounds = number[]
type HexColor = string

export interface HighlightSpecsBounds {
    strings: Bounds[],
    comments: Bounds[],
    keywords: Bounds[]
}

export interface HighlightSpecs {
    code: string,
    bounds: HighlightSpecsBounds
}

export interface Theme {
    strings: HexColor,
    comments: HexColor,
    keywords: HexColor,
}

export const convert = ({ bounds, code } : {bounds: HighlightSpecsBounds, code: string}): HighlightSpecsBounds => {
    const newBounds: HighlightSpecsBounds = {} as HighlightSpecsBounds;

    const lines = code.split('\n');

    for (const type of Object.keys(bounds)) {
        for (const foo of bounds[type as keyof HighlightSpecsBounds]) {
            const coord = foo as unknown as Bounds[]; // ts thinks coord is a bounds instaid of a Bounds[]
            const beginIndex = lines.slice(0, coord[0][0]).reduce((total, line) => total + line.length, 0) + (coord[0][0]) * '\n'.length + coord[0][1];
            const endIndex = lines.slice(0, coord[1][0]).reduce((total, line) => total + line.length, 0) + (coord[1][0]) * '\n'.length + coord[1][1] + 1;

            if (newBounds[type as keyof HighlightSpecsBounds] === undefined) {
                newBounds[type as keyof HighlightSpecsBounds] = [];
            }

            newBounds[type as keyof HighlightSpecsBounds].push([beginIndex, endIndex]);
        }
    }

    return newBounds;
}

const Header = ({ fileName }: { fileName: string }) => {
    return <>
        <div className="bg-neutral-900 p-2 pb-0 flex flex-row justify-between items-center">
            <div className="bg-neutral-600 p-1 pl-2 pr-2 cursor-pointer flex items-center justify-center gap-1 rounded-t-md">
                <img src="./python.png" alt="Python file" className="w-3 h-3" />
                {fileName}
            </div>
            <div className="flex flex-row gap-1 items-center justify-center pb-2">
                <div className="rounded-full w-4 h-4 bg-red-500 cursor-pointer" />
                <div className="rounded-full w-4 h-4 bg-orange-500 cursor-pointer" />
                <div className="rounded-full w-4 h-4 bg-green-500 cursor-pointer" />
            </div>
        </div>
    </>
}

// Inspiration taken from examples, found at: https://www.npmjs.com/package/react-dropzone
function Dropzone({ setHighlightSpecs }: { setHighlightSpecs: (raw: string) => void }) {
    const onDrop = useCallback((acceptedFiles: any) => {
      acceptedFiles.forEach((file: any) => {
        const reader = new FileReader()
  
        reader.onabort = () => console.log('file reading was aborted')
        reader.onerror = () => console.log('file reading has failed')
        reader.onload = () => setHighlightSpecs(reader.result as string)

        reader.readAsText(file)
      })
      
    }, [setHighlightSpecs])
    
    const {getRootProps, getInputProps} = useDropzone({onDrop})
    return (
      <div {...getRootProps()} className="bg-neutral-800 w-full h-full flex items-center justify-center text-white">
        <input {...getInputProps()} />
        <p>Drag 'n drop your input file here</p>
      </div>
    )
  }

const App = () => {
    const [highlightSpecs, setHighlightSpecs] = useState<HighlightSpecs | null>(null)

    const themes: Theme[] = [
        {
            strings: "#97c279",
            comments: "#7b808a",
            keywords: "#c678dd"
        }
    ] 

    return (
        <div className="flex flex-col w-screen h-screen text-white">
            <Header fileName="main.py" />
            {
                !highlightSpecs 
                ? <Dropzone setHighlightSpecs={(raw: string) => setHighlightSpecs(JSON.parse(raw))}></Dropzone> 
                : <Editor highlightSpecs={highlightSpecs} theme={themes[0]} />
            }
        </div>
  );
}

export default App