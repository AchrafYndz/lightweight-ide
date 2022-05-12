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

type Bounds = number[]

export interface HighlightSpecs {
    code: string,
    strings: Bounds[],
    comments: Bounds[],
    keywords: Bounds[]
}

const App = () => {
  return (
    <div className="w-full h-full text-white">
        <Header fileName="main.py" />
        <Editor />
    </div>
  );
}

export default App