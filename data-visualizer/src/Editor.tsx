import { HighlightSpecs, HighlightSpecsBounds, Theme, HighlightSpecsRaw, HighlightSpecsBoundsRaw } from "./App"

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

const convert = ({ bounds, code } : {bounds: HighlightSpecsBoundsRaw, code: string}): HighlightSpecsBounds => {
    const newBounds = {} as HighlightSpecsBounds;

    const lines = code.split('\n');

    for (const type of Object.keys(bounds)) {
        for (const coords of bounds[type as keyof HighlightSpecsBounds]) {
            const start = coords[0]
            const end = coords[1]

            const beginIndex = lines.slice(0, start[0]).reduce((total, line) => total + line.length, 0) + (start[0]) * '\n'.length + start[1];
            const endIndex = lines.slice(0, end[0]).reduce((total, line) => total + line.length, 0) + (end[0]) * '\n'.length + end[1] + 1;

            if (newBounds[type as keyof HighlightSpecsBounds] === undefined) {
                newBounds[type as keyof HighlightSpecsBounds] = [];
            }

            newBounds[type as keyof HighlightSpecsBounds].push([beginIndex, endIndex]);
        }
    }

    return newBounds;
}

const Editor = ({ highlightSpecs, theme }: { highlightSpecs: HighlightSpecsRaw, theme: Theme }) => {
    if (!highlightSpecs) return <></>
    const lines = highlightSpecs.code.split("\n")

    const newHighlightSpecs: HighlightSpecs = {
        bounds: convert(highlightSpecs), 
        code: highlightSpecs.code
    }

    console.log(newHighlightSpecs.bounds);

    return <>
        <div className="flex flex-col w-full h-full bg-neutral-800 p-5">
            {
                lines.map((line, i) => {
                    const startIndex = lines.slice(0, i).reduce((total, line) => total + line.length, 0) + i * "\n".length;
                    return <div className="flex gap-10" key={i}>
                                <div className="w-4 overflow-hidden text-right">{i + 1}</div>
                                <Line highlightSpecsBounds={newHighlightSpecs.bounds} theme={theme} startIndex={startIndex} text={line}></Line>
                    </div>
                })
            }
        </div>
    </>
}

export default Editor