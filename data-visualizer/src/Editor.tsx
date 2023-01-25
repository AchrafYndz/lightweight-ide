import { MutableRefObject, useEffect, useRef } from "react";

type HexColor = string;
interface Theme {
  comments: HexColor
  keywords: HexColor
  identifier: HexColor
  function_call_identifier: HexColor
  literals: HexColor
  function_name: HexColor
  arguments: HexColor
}

type Bounds = number[];

interface HighlightSpecsBounds {
  comments: Bounds[]
  keywords: Bounds[]
  identifier: Bounds[]
  function_call_identifier: Bounds[]
  function_name: Bounds[]
  literals: Bounds[]
  arguments: Bounds[]
}

interface Error {
  line: number
  message: string
}

interface HighlightSpecs {
  code: string
  bounds: HighlightSpecsBounds
  errors: Error[]
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

  for (const error of spec.errors) {
    const split = html.split("\n")
    if (split.length <= error.line) continue;

    split[error.line] += `  <span&nbspp;style="color:red;">[ERROR] ${error.message}</span>`
    html = split.join("\n")
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
    comments: "#7B808A",
    keywords: "#C678DD",
    identifier: "#ABB2BF",
    literals: "#E5C07B",
    function_call_identifier: "196CF0",
    function_name: "#62AFF0",
    arguments: "#E5C07B",
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
            <div className="w-2 flex justify-end">{t}</div>
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
