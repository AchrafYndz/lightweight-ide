import React from 'react';

const Header = () => {
    const fileName = "main.py"
    
    return <>
        <div className="bg-neutral-900 p-2 pb-0 flex flex-row justify-between items-center">
            <div className="bg-neutral-600 p-1 pl-2 pr-2">{fileName}</div>
            <div className="flex flex-row gap-1 items-center justify-center pb-2">
                <div className="rounded-full w-4 h-4 bg-red-500" />
                <div className="rounded-full w-4 h-4 bg-orange-500" />
                <div className="rounded-full w-4 h-4 bg-green-500" />
            </div>
        </div>
    </>
}

const App = () => {
  return (
    <div className="w-full h-full text-white">
        <Header />
    </div>
  );
}

export default App