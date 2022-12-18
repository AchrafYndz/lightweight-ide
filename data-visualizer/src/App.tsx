import Editor from "./Editor"

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

const App = () => {
    return (
        <div className="flex flex-col w-screen h-screen text-white">
            <Header fileName="main.py" />
            <Editor />
        </div>
  );
}

export default App