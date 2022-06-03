import { HighlightSpecs, HighlightSpecsBounds, Theme } from "./App"

const Line = ({ highlightSpecsBounds, theme, startIndex, text }: { highlightSpecsBounds: HighlightSpecsBounds, theme: Theme, startIndex: number, text: string }) => {
    let n: JSX.Element[] = [];
    let content = "";
    let currentColor: string | null = null;

    const pushContent = () => {
        if (content.length === 0) return
        n.push(<span key={n.length} style={{ ...(currentColor && { color: currentColor }) }}>{content}</span>)
        content = ""
    }

    for (let i = 0; i < text.length; i++) {
        for (const type of Object.keys(highlightSpecsBounds)) {
            if (highlightSpecsBounds[type as keyof HighlightSpecsBounds].find(bounds => bounds[0] === (i + startIndex))) {
                pushContent()
                currentColor = theme[type as keyof Theme];
            }

            if (highlightSpecsBounds[type as keyof HighlightSpecsBounds].find(bounds => bounds[1] === (i + startIndex))) {
                pushContent()
                currentColor = null;
            }
        }

        content += text[i]
    }

    pushContent()

    return <p className="whitespace-pre">{n}</p>
}

const Editor = ({ highlightSpecs, theme }: { highlightSpecs: HighlightSpecs, theme: Theme }) => {
    if (!highlightSpecs) return <></>
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