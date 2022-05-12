import Editor from "./Editor"

const Header = ({ fileName }: { fileName: string }) => {
    return <>
        <div className="bg-neutral-900 p-2 pb-0 flex flex-row justify-between items-center">
            <div className="bg-neutral-600 p-1 pl-2 pr-2 cursor-pointer">{fileName}</div>
            <div className="flex flex-row gap-1 items-center justify-center pb-2">
                <div className="rounded-full w-4 h-4 bg-red-500 cursor-pointer" />
                <div className="rounded-full w-4 h-4 bg-orange-500 cursor-pointer" />
                <div className="rounded-full w-4 h-4 bg-green-500 cursor-pointer" />
            </div>
        </div>
    </>
}

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

const App = () => {
    const highlightSpecs: HighlightSpecs = {
        "code": "from string import ascii_lowercase\n\ndef main():\n    # This is a comment!\n    message = \"Hello, alphabet!\"\n    print(message, ascii_lowercase)\n\nmain()",
        "bounds": {
            "strings": [
                [87, 105]
            ],
            "comments": [
                [52, 72]
            ],
            "keywords": [
                [0, 4],
                [12, 18],
                [36, 39]
            ]
        }
    }
    // dodododoIII

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
            <Editor highlightSpecs={highlightSpecs} theme={themes[0]} />
        </div>
  );
}

export default App