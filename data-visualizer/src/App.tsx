import Editor from "./Editor";

const Header = ({ fileName }: { fileName: string }) => {
  return (
    <>
      <div className="flex flex-row items-center justify-between bg-neutral-900 p-2 pb-0">
        <div className="flex cursor-pointer items-center justify-center gap-1 rounded-t-md bg-neutral-600 p-1 pl-2 pr-2">
          <img src="./cmm.png" alt="CMM file" className="h-4 w-4" />
          {fileName}
        </div>
        <div className="flex flex-row items-center justify-center gap-1 pb-2">
          <div className="h-4 w-4 cursor-pointer rounded-full bg-red-500" />
          <div className="h-4 w-4 cursor-pointer rounded-full bg-orange-500" />
          <div className="h-4 w-4 cursor-pointer rounded-full bg-green-500" />
        </div>
      </div>
    </>
  );
};

const App = () => {
  return (
    <div className="flex h-screen w-screen flex-col text-white">
      <Header fileName="main.cmm" />
      <Editor />
    </div>
  );
};

export default App;
