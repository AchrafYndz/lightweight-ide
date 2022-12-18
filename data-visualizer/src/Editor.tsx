import { MutableRefObject, useEffect, useRef } from "react";

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

const highlight = async (code: string): Promise<HighlightSpecs> => {
  const response = await fetch("http://0.0.0.0:18080/json", {
    method: "POST",
    headers: {
      "content-type": "text/plain",
    },
    body: code,
  });

  return response.json();
};

const f = async (
  content: string,
  ref: MutableRefObject<HTMLDivElement>,
  theme: Theme
) => {

  const spec = await highlight(content);

  const merged = [];
  for (const key of Object.keys(spec.bounds))
    merged.push(
      ...spec.bounds[key as keyof HighlightSpecsBounds].map((c) => ({
        bounds: c,
        color: theme[key as keyof Theme],
      }))
    );
  merged.sort((a, b) => a.bounds[0] - b.bounds[0]);

  let html = content;
  let offset = 0;
  for (const pair of merged) {
    pair.bounds[0] += offset;
    pair.bounds[1] += offset;
    const original = html.substring(pair.bounds[0], pair.bounds[1]);
    const target = `<span&nbspp;style="color:${pair.color};">${original}</span>`;
    html =
      html.substring(0, pair.bounds[0]) +
      target +
      html.substring(pair.bounds[1], html.length);

    offset += target.length - original.length;
  }

  html = html.replace(/ /g, "&nbsp;");
  html = html.replace(/&nbspp;/g, " ");
  html = html.replace(/\n/g, "<br>");

  ref.current.innerHTML = html;
};

// Source (modified): https://stackoverflow.com/questions/6637341/use-tab-to-indent-in-textarea
const tabHandler = (e: KeyboardEvent, textarea: HTMLTextAreaElement) => {
  if (e.key !== "Tab") return;
  e.preventDefault();

  // Get cursor location
  const start = textarea.selectionStart;
  const end = textarea.selectionEnd;

  // Insert tab
  textarea.value =
    textarea.value.substring(0, start) + "    " + textarea.value.substring(end);

  textarea.selectionStart = textarea.selectionEnd = start + 4;
};

const Editor = () => {
  const theme: Theme = {
    strings: "#97c279",
    comments: "#7b808a",
    keywords: "#c678dd",
  };

  const mirrorRef = useRef() as MutableRefObject<HTMLDivElement>;
  const inputRef = useRef() as MutableRefObject<HTMLTextAreaElement>;

  useEffect(() => {
    if (!inputRef.current) return;
    const current = inputRef.current;
    const tabHandlerCurrent = (e: KeyboardEvent) => tabHandler(e, current);

    current.addEventListener("keydown", tabHandlerCurrent);

    return () => {
      current.removeEventListener("keydown", tabHandlerCurrent);
    };
  }, [inputRef]);

  return (
    <>
      <div className="flex flex-row w-full bg-neutral-800 p-5 gap-2">
        <div className="flex flex-col text-gray-400">
          {[...Array(100).keys()].map((t) => (
            <div className="w-2 flex items-end justify-end">{t}</div>
          ))}
        </div>
        <div className="flex w-full h-full relative">
          <textarea
              ref={inputRef}
              onInput={(e) => f(e.currentTarget.value, mirrorRef, theme)}
              className="w-full h-full bg-neutral-800 focus:outline-none z-10 text-transparent bg-transparent caret-white"
            />
          <div
            ref={mirrorRef}
            className="w-full h-full bg-neutral-800 absolute"
          />
        </div>
      </div>
    </>
  );
};

export default Editor;
