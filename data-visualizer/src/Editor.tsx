import { HighlightSpecs, HighlightSpecsBounds, Theme } from "./App"

const Line = ({ highlightSpecsBounds, theme, startIndex, text }: { highlightSpecsBounds: HighlightSpecsBounds, theme: Theme, startIndex: number, text: string }) => {
    let localBounds: HighlightSpecsBounds = highlightSpecsBounds;
    
    for (const [key, occurrences] of Object.entries(highlightSpecsBounds)) {
        const currentType = key as keyof HighlightSpecsBounds;
        localBounds[currentType] = [];

        for (const occurence of occurrences) {
            const oStart = occurence[0]
            const oEnd = occurence[1]

            if (oStart >= startIndex && startIndex + text.length >= oEnd) {
                const lStart = oStart - startIndex
                const lEnd = oEnd - startIndex + 1

                localBounds[currentType].push([lStart, lEnd])
            }
        }
    }

    let n: JSX.Element[] = [];
    let content = "";

    const pushContent = () => {
        if (content.length === 0) return
        n.push(<span>{content}</span>)
        content = ""
    }
    
    for (let i = 0; i < text.length; i++) {
        for (const type of Object.keys(localBounds)) {
            if (localBounds[type as keyof HighlightSpecsBounds].find(bounds => bounds[0] === i)) {
                pushContent()
            }

            if (localBounds[type as keyof HighlightSpecsBounds].find(bounds => bounds[1] === i)) {
                n.push(<span style={{color: theme[type as keyof Theme] as any}}>{content}</span>)
                content = ""
            }
        }
        // if (localBounds.find())
        content += text[i]
    }

    pushContent()

    return <p>
        {n}
    </p>
}

const Editor = ({ highlightSpecs, theme }: { highlightSpecs: HighlightSpecs, theme: Theme }) => {
    const lines = highlightSpecs.code.split("\n")

    return <>
        <div className="flex flex-col w-full h-full bg-neutral-800 p-5">
            {
                lines.map((line, i) => {
                    const startIndex = lines.slice(0, i).reduce((total, line) => total + line.length, 0) + i * "\n".length;
                    return <div className="flex gap-10" key={i}>
                                <div className="w-4 overflow-hidden text-right">{i + 1}</div>
                                <Line highlightSpecsBounds={highlightSpecs.bounds} theme={theme} startIndex={startIndex} text={line}></Line>
                    </div>
                })
            }
        </div>
    </>
}

export default Editor