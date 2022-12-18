import { MutableRefObject, RefObject, useEffect, useRef, useState } from "react";
import { reduceEachLeadingCommentRange } from "typescript";

type HexColor = string;
export interface Theme {
  strings: HexColor;
  comments: HexColor;
  keywords: HexColor;
}

export type Bounds = number[];
export type Pair = Bounds[];

export interface HighlightSpecsBounds {
  strings: Bounds[];
  comments: Bounds[];
  keywords: Bounds[];
}

export interface HighlightSpecs {
  code: string;
  bounds: HighlightSpecsBounds;
}


// const Line = ({
//   highlightSpecsBounds,
//   theme,
//   startIndex,
//   text,
// }: {
//   highlightSpecsBounds: HighlightSpecsBounds;
//   theme: Theme;
//   startIndex: number;
//   text: string;
// }) => {
//   let n: JSX.Element[] = [];
//   let content = "";
//   let currentColor: string | null = null;

//   const pushContent = () => {
//     if (content.length === 0) return;
//     n.push(
//       <span
//         key={n.length}
//         style={{ ...(currentColor && { color: currentColor }) }}
//       >
//         {content}
//       </span>
//     );
//     content = "";
//   };

//   for (let i = 0; i < text.length; i++) {
//     for (const type of Object.keys(highlightSpecsBounds)) {
//       if (
//         highlightSpecsBounds[type as keyof HighlightSpecsBounds].find(
//           (bounds) => bounds[0] === i + startIndex
//         )
//       ) {
//         pushContent();
//         currentColor = theme[type as keyof Theme];
//       }

//       if (
//         highlightSpecsBounds[type as keyof HighlightSpecsBounds].find(
//           (bounds) => bounds[1] === i + startIndex
//         )
//       ) {
//         pushContent();
//         currentColor = null;
//       }
//     }

//     content += text[i];
//   }

//   pushContent();

//   return <p className="whitespace-pre">{n}</p>;
// };

// const convert = ({
//   bounds,
//   code,
// }: {
//   bounds: HighlightSpecsBoundsRaw;
//   code: string;
// }): HighlightSpecsBounds => {
//   const newBounds = {} as HighlightSpecsBounds;
//   const lines = code.split("\n");

//   for (const type of Object.keys(bounds)) {
//     // Let's start off by pre-processing the data
//     // We have to make sure that all pairs are on the same line before continueing
//     const boundsOriginal = bounds[type as keyof HighlightSpecsBounds];
//     const boundsNew = [];

//     for (const pair of boundsOriginal) {
//       if (pair[0][0] === pair[1][0]) boundsNew.push(pair);
//       else {
//         for (let i = pair[0][0]; i <= pair[1][0]; i++) {
//           // Start
//           if (i === 0)
//             boundsNew.push([
//               [i, pair[0][1]],
//               [i, lines[i].length - 1],
//             ]);
//           // End
//           else if (i === pair[1][0])
//             boundsNew.push([
//               [i, 0],
//               [i, pair[1][1]],
//             ]);
//           // Other
//           else
//             boundsNew.push([
//               [i, 0],
//               [i, lines[i].length - 1],
//             ]);
//         }
//       }
//     }

//     // Convert to indexes
//     for (const coords of boundsNew) {
//       const start = coords[0];
//       const end = coords[1];

//       const beginIndex =
//         lines
//           .slice(0, start[0])
//           .reduce((total, line) => total + line.length, 0) +
//         start[0] * "\n".length +
//         start[1];
//       const endIndex =
//         lines.slice(0, end[0]).reduce((total, line) => total + line.length, 0) +
//         end[0] * "\n".length +
//         end[1] +
//         1;

//       newBounds[type as keyof HighlightSpecsBounds] ??= [];
//       newBounds[type as keyof HighlightSpecsBounds].push([
//         beginIndex,
//         endIndex,
//       ]);
//     }
//   }

//   return newBounds;
// };

const highlight = async (code: string): Promise<HighlightSpecs> => {
  const response = await fetch("http://0.0.0.0:18080/json", {
    method: "POST",
    headers: {
      "content-type": "text/plain"
    },
    body: code
  })

  return response.json()
}


const f = async (e: KeyboardEvent) => {
  // console.log(e)
  if ((e.ctrlKey || e.metaKey) && e.key === "s") {
    console.log("hey")
    // Prevent the Save dialog to open
    e.preventDefault();
    // Place your code here
    // setHighlightSpecs(await highlight(code))
    const s = await highlight(content)
    // console.log(s)
    let html = content
    console.log(html)
    
    const merged = []
    for (const key of Object.keys(s.bounds)) merged.push(...(s.bounds[key as keyof HighlightSpecsBounds].map(c => ({bounds: c, color: theme[key as keyof Theme]}))))
    merged.sort((a, b) => a.bounds[0] - b.bounds[0])
    
    let offset = 0
    for (const pair of merged) {
      pair.bounds[0] += offset
      pair.bounds[1] += offset
      const original = html.substring(pair.bounds[0], pair.bounds[1])
      const target = `<span&nbspp;style="color:${pair.color};">${original}</span>`
      // const target = original
      // const target = html.substring(pair.bounds[0], pair.bounds[1])
      html = html.substring(0, pair.bounds[0]) + target + html.substring(pair.bounds[1], html.length) 

      offset += target.length - original.length
    }

    // console.log(html)

    html = html.replace(/ /g, "&nbsp;")
    html = html.replace(/&nbspp;/g, " ")
    html = html.replace(/\n/g, "<br>")
    // console.log(html === ref.current.innerText)

    // console.log(html)
    // ref.current.innerHTML = html
    ref.current.innerHTML = html
    console.log(ref.current.innerHTML)


    // ref.current.innerHTML = ref.current.innerHTML.replace("banana", "<span style=\"background: red;\">banana</span>")
  }
}

const Editor = () => {
  const theme: Theme = {
      strings: "#97c279",
      comments: "#7b808a",
      keywords: "#c678dd",
    }
  


  // if (!highlightSpecs) return <></>
  const ref = useRef() as MutableRefObject<HTMLDivElement>;
  const [content, setContent] = useState("")

  useEffect(() => {
   
    document.addEventListener("keydown", f)

    return () => {
      document.removeEventListener("keydown", f)
    }
  }, [content]);

  return (
    <>
      <div className="flex flex-col w-full h-full bg-neutral-800 p-5 overflow-scroll relative">
          <div ref={ref}
          className="w-full h-full bg-neutral-800 focus:outline-none fixed">
            
          </div>
          <textarea
            // ref={ref}
            onInput={e => setContent(e.currentTarget.value)}
            className="w-full h-full bg-neutral-800 focus:outline-none z-10"
          ></textarea>
      </div>
    </>
  );
};

export default Editor;
