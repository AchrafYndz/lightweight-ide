import { MutableRefObject, useEffect, useRef } from "react";

type HexColor = string;
interface Theme {
  keyword: HexColor;
  identifier: HexColor;
  function_call_identifier: HexColor;
  literal: HexColor;
  function_name: HexColor;
  argument: HexColor;
}

type ConvertedBounds = number[];
interface BackendBounds {
  end: number[];
  start: number[];
}

interface HighlightSpecsBounds<T> {
  keyword: T[];
  identifier: T[];
  function_call_identifier: T[];
  function_name: T[];
  literal: T[];
  argument: T[];
}

interface Error {
  line: number;
  message: string;
  corrections: string[];
}

interface HighlightSpecs<T> {
  code: string;
  bounds: HighlightSpecsBounds<T>;
  errors: Error[];
}

const convert_backend_bounds = (
  bounds: BackendBounds,
  code: string,
): ConvertedBounds => {
  if (bounds.start.every((n) => n === 0) && bounds.end.every((n) => n === 0))
    return [0, 0] as ConvertedBounds;

  let index = 0;
  let row = 0;
  let col = 0;

  let start = 0;
  let end = 0;

  const chars = [...code];
  for (const c of chars) {
    switch (c) {
      case "\n": {
        col = 0;
        row += 1;
        break;
      }
      default: {
        col += 1;
      }
    }

    index += 1;

    if (([row, col] as number[]).every((n, i) => n === bounds.start[i])) {
      start = index;
    }

    if (([row, col] as number[]).every((n, i) => n === bounds.end[i])) {
      end = index + 1;
      return [start, end] as ConvertedBounds;
    }
  }

  console.log(bounds);
  throw new Error("failed to find start and end index of bounds");
};

const highlight = async (
  code: string,
): Promise<HighlightSpecs<ConvertedBounds>> => {
  const response = await fetch("http://0.0.0.0:18080/json", {
    method: "POST",
    headers: {
      "content-type": "text/plain",
    },
    body: JSON.stringify({ code: code }),
  });

  const parsed = (await response.json()) as HighlightSpecs<BackendBounds>;

  const converted: HighlightSpecsBounds<ConvertedBounds> = {
    keyword: [],
    identifier: [],
    function_call_identifier: [],
    function_name: [],
    literal: [],
    argument: [],
  };

  if (parsed.bounds) {
    for (const key of Object.keys(parsed.bounds)) {
      converted[key as keyof HighlightSpecsBounds<ConvertedBounds>] =
        parsed.bounds[key as keyof HighlightSpecsBounds<BackendBounds>]
          .map((b) => convert_backend_bounds(b, code))
          .filter((a) => a[0] !== a[1]);
    }
  }

  return { ...parsed, bounds: converted } as HighlightSpecs<ConvertedBounds>;
};

const processCode = async (
  content: string,
  ref: MutableRefObject<HTMLDivElement>,
  theme: Theme,
) => {
  const spec = await highlight(content);

  const merged = [];
  if (spec.bounds) {
    for (const key of Object.keys(spec.bounds))
      merged.push(
        ...spec.bounds[key as keyof HighlightSpecsBounds<ConvertedBounds>].map(
          (c) => ({
            bounds: c,
            color: theme[key as keyof Theme],
          }),
        ),
      );
    merged.sort((a, b) => a.bounds[0] - b.bounds[0]);
  }

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

  if (spec.errors) {
    for (const error of spec.errors) {
      const split = html.split("\n");
      if (split.length <= error.line) continue;

      split[error.line] +=
        `  <span&nbspp;style="color:red;">[ERROR] ${error.message}</span>`;
      html = split.join("\n");

      console.log("possible corrections:", error.corrections);
    }
  }

  html = html.replace(/ /g, "&nbsp;");
  html = html.replace(/&nbspp;/g, " ");
  html = html.replace(/\n/g, "<br>");

  ref.current.innerHTML = html;
};

// Source (modified): https://stackoverflow.com/questions/6637341/use-tab-to-indent-in-textarea
const tabHandler = (
  e: KeyboardEvent,
  textarea: HTMLTextAreaElement,
  cb: () => void,
) => {
  if (e.key !== "Tab") return;
  e.preventDefault();

  // Get cursor location
  const start = textarea.selectionStart;
  const end = textarea.selectionEnd;

  // Insert tab
  textarea.value =
    textarea.value.substring(0, start) + "    " + textarea.value.substring(end);

  textarea.selectionStart = textarea.selectionEnd = start + 4;
  cb();
};

const theme: Theme = {
  keyword: "#C678DD",
  identifier: "#ABB2BF",
  literal: "#E5C07B",
  function_call_identifier: "#89B4FA",
  function_name: "#89B4FA",
  argument: "#E5C07B",
};

const Editor = () => {
  const mirrorRef = useRef() as MutableRefObject<HTMLDivElement>;
  const inputRef = useRef() as MutableRefObject<HTMLTextAreaElement>;

  useEffect(() => {
    if (!inputRef.current) return;
    const current = inputRef.current;
    const tabHandlerCurrent = (e: KeyboardEvent) => {
      tabHandler(e, current, () =>
        processCode(inputRef.current.value, mirrorRef, theme),
      );
    };

    current.addEventListener("keydown", tabHandlerCurrent);

    return () => {
      current.removeEventListener("keydown", tabHandlerCurrent);
    };
  }, [inputRef]);

  return (
    <>
      <div className="flex w-full flex-row gap-2 bg-neutral-800 p-5">
        <div className="flex flex-col text-gray-400">
          {[...Array(100).keys()].map((t) => (
            <div className="flex w-2 justify-end">{t}</div>
          ))}
        </div>
        <div className="relative flex h-full w-full" spellCheck={false}>
          <textarea
            ref={inputRef}
            onInput={(e) =>
              processCode(e.currentTarget.value, mirrorRef, theme)
            }
            className="z-10 h-full w-full bg-neutral-800 bg-transparent text-transparent caret-white focus:outline-none"
          />
          <div
            ref={mirrorRef}
            className="absolute h-full w-full whitespace-nowrap bg-neutral-800"
          />
        </div>
      </div>
    </>
  );
};

export default Editor;
