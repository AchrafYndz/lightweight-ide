import { HighlightSpecs, HighlightSpecsBounds, Theme } from "./App"

const Line = ({ highlightSpecsBounds, themes, startIndex, text }: { highlightSpecsBounds: HighlightSpecsBounds, themes: Theme[], startIndex: number, text: string }) => {
    let localBounds: HighlightSpecsBounds = highlightSpecsBounds;
    
    for (const [key, occurrences] of Object.entries(highlightSpecsBounds)) {
        const currentType = key as keyof HighlightSpecsBounds;
        localBounds[currentType] = [];

        for (const occurence of occurrences) {
            const oStart = occurence[0]
            const oEnd = occurence[1]

            if (oStart >= startIndex && startIndex + text.length >= oEnd) {
                const lStart = oStart - startIndex
                const lEnd = oEnd - startIndex

                localBounds[currentType].push([lStart, lEnd])
            }
        }
    }

    let n = "";
    for (let i = 0; i < text.length; i++) {
        n += text[i]
    }
    
    return <>
        {startIndex}
    </>
}

const Editor = ({ highlightSpecs, themes }: { highlightSpecs: HighlightSpecs, themes: Theme[] }) => {
    const lines = highlightSpecs.code.split("\n")

    return <>
        <div className="flex flex-col w-full h-full bg-neutral-800 p-5">
            {
                lines.map((line, i) => {
                    const startIndex = lines.slice(0, i).reduce((total, line) => total + line.length, 0) + i * "\n".length;
                    return <div className="flex gap-10" key={i}>
                                <div className="w-4 overflow-hidden text-right">{i + 1}</div>
                                <Line highlightSpecs={highlightSpecs} themes={themes} startIndex={startIndex} text={line}></Line>
                    </div>
                })
            }
        </div>
    </>
}

export default Editor