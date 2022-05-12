import { HighlightSpecs, Theme } from "./App"

const Line = ({ highlightSpecs, themes, index, text }: { highlightSpecs: HighlightSpecs, themes: Theme[], index: number, text: string }) => {
    for (const [key, occurrences] of Object.entries(highlightSpecs)) {

    }
    
    return <>
        {text}
    </>
}

const Editor = ({ highlightSpecs, themes }: { highlightSpecs: HighlightSpecs, themes: Theme[] }) => {
    const lines = highlightSpecs.code.split("\n")

    return <>
        <div className="flex flex-col w-full h-full bg-neutral-800 p-5">
            {
                lines.map((line, i) => {
                    return <div className="flex gap-10">
                                <div className="w-4 overflow-hidden text-right">{i + 1}</div>
                                <Line highlightSpecs={highlightSpecs} themes={themes} index={i} text={line}></Line>
                    </div>
                })
            }
        </div>
    </>
}

export default Editor