import { HighlightSpecs, Theme } from "./App"

const Editor = ({ highlightSpecs, themes }: { highlightSpecs: HighlightSpecs, themes: Theme[] }) => {
    const lines = highlightSpecs.code.split("\n")

    return <>
        <div className="flex flex-col w-full h-full bg-neutral-800 p-5">
            {
                lines.map((line, t) => {
                    return <div className="flex gap-10">
                        <div className="w-4 overflow-hidden text-right">{t + 1}</div>
                        <div>{line}</div>
                    </div>
                })
            }
        </div>
    </>
}

export default Editor