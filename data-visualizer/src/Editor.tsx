import { useEffect, useState } from "react";

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

export interface HighlightSpecsBoundsRaw {
  strings: Pair[];
  comments: Pair[];
  keywords: Pair[];
}

export interface HighlightSpecs {
  code: string;
  bounds: HighlightSpecsBounds;
}

export interface HighlightSpecsRaw {
  code: string;
  bounds: HighlightSpecsBoundsRaw;
}

const Line = ({
  highlightSpecsBounds,
  theme,
  startIndex,
  text,
}: {
  highlightSpecsBounds: HighlightSpecsBounds;
  theme: Theme;
  startIndex: number;
  text: string;
}) => {
  let n: JSX.Element[] = [];
  let content = "";
  let currentColor: string | null = null;

  const pushContent = () => {
    if (content.length === 0) return;
    n.push(
      <span
        key={n.length}
        style={{ ...(currentColor && { color: currentColor }) }}
      >
        {content}
      </span>
    );
    content = "";
  };

  for (let i = 0; i < text.length; i++) {
    for (const type of Object.keys(highlightSpecsBounds)) {
      if (
        highlightSpecsBounds[type as keyof HighlightSpecsBounds].find(
          (bounds) => bounds[0] === i + startIndex
        )
      ) {
        pushContent();
        currentColor = theme[type as keyof Theme];
      }

      if (
        highlightSpecsBounds[type as keyof HighlightSpecsBounds].find(
          (bounds) => bounds[1] === i + startIndex
        )
      ) {
        pushContent();
        currentColor = null;
      }
    }

    content += text[i];
  }

  pushContent();

  return <p className="whitespace-pre">{n}</p>;
};

const convert = ({
  bounds,
  code,
}: {
  bounds: HighlightSpecsBoundsRaw;
  code: string;
}): HighlightSpecsBounds => {
  const newBounds = {} as HighlightSpecsBounds;
  const lines = code.split("\n");

  for (const type of Object.keys(bounds)) {
    // Let's start off by pre-processing the data
    // We have to make sure that all pairs are on the same line before continueing
    const boundsOriginal = bounds[type as keyof HighlightSpecsBounds];
    const boundsNew = [];

    for (const pair of boundsOriginal) {
      if (pair[0][0] === pair[1][0]) boundsNew.push(pair);
      else {
        for (let i = pair[0][0]; i <= pair[1][0]; i++) {
          // Start
          if (i === 0)
            boundsNew.push([
              [i, pair[0][1]],
              [i, lines[i].length - 1],
            ]);
          // End
          else if (i === pair[1][0])
            boundsNew.push([
              [i, 0],
              [i, pair[1][1]],
            ]);
          // Other
          else
            boundsNew.push([
              [i, 0],
              [i, lines[i].length - 1],
            ]);
        }
      }
    }

    // Convert to indexes
    for (const coords of boundsNew) {
      const start = coords[0];
      const end = coords[1];

      const beginIndex =
        lines
          .slice(0, start[0])
          .reduce((total, line) => total + line.length, 0) +
        start[0] * "\n".length +
        start[1];
      const endIndex =
        lines.slice(0, end[0]).reduce((total, line) => total + line.length, 0) +
        end[0] * "\n".length +
        end[1] +
        1;

      newBounds[type as keyof HighlightSpecsBounds] ??= [];
      newBounds[type as keyof HighlightSpecsBounds].push([
        beginIndex,
        endIndex,
      ]);
    }
  }

  return newBounds;
};

const highlight = async (code: string) => {
  fetch("http://0.0.0.0:18080/json")
}

const Editor = () => {
  const theme: Theme[] = [
    {
      strings: "#97c279",
      comments: "#7b808a",
      keywords: "#c678dd",
    },
  ];

  const [highlightSpecs, setHighlightSpecs] = useState<HighlightSpecsRaw>({
    code: "",
    bounds: { strings: [], comments: [], keywords: [] },
  });
  // if (!highlightSpecs) return <></>
  const lines = highlightSpecs.code.split("\n");
  const [code, setCode] = useState("");

  const newHighlightSpecs: HighlightSpecs = {
    bounds: convert(highlightSpecs),
    code: highlightSpecs.code,
  };

  useEffect(() => {
    document.addEventListener("keydown", (e) => {
      // console.log(e)
      if ((e.ctrlKey || e.metaKey) && e.key === "s") {
        // Prevent the Save dialog to open
        e.preventDefault();
        // Place your code here
        console.log("CTRL + S");
      }
    });
  }, []);

  return (
    <>
      <div className="flex flex-col w-full h-full bg-neutral-800 p-5 overflow-scroll">
        {/* {
                lines.map((line, i) => {
                    const startIndex = lines.slice(0, i).reduce((total, line) => total + line.length, 0) + i * "\n".length;
                    return <div className="flex gap-10" key={i}>
                                <div className="w-5 overflow-hidden text-right">{i + 1}</div>
                                <Line highlightSpecsBounds={newHighlightSpecs.bounds} theme={theme} startIndex={startIndex} text={line}></Line>
                    </div>
                })
            } */}
        <textarea
          onChange={(e) => setCode(e.target.value)}
          className="w-full h-full bg-neutral-800 focus:outline-none"
        ></textarea>
      </div>
    </>
  );
};

export default Editor;
