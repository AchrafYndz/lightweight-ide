import React from 'react';

const Header = () => {
    const fileName = "main.py"
    
    return <>
        <div className="bg-neutral-900 p-2 pb-0 flex flex-row justify-between">
            <div className="bg-neutral-600 p-1 pl-2 pr-2">{fileName}</div>
            <div>

            </div>
        </div>
    </>
}

const App = () => {
  return (
    <div className="w-full h-full">
        <Header />
    </div>
  );
}

export default App