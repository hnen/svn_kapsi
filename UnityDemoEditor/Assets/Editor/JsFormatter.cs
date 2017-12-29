using UnityEngine;
using System.Collections.Generic;
using System.Text;
using System;
using System.IO;
using System.Linq;

public class JsFormatter {

	const int TAB_WIDTH = 4;

	public class JsBlock {
		public string [] texts;
		public JsBlock [] childBlocks;
	}

	public static string Format(string input) {
		input = RemoveWhitespaces(input);
		//input = AddBracketNewlines(input);
		input = AddWhiteSpaces(input);
		//input = AddIndent(input);

		JsBlock rootBlock = ParseBlocks(input);	

		MemoryStream ms = new MemoryStream();
		StreamWriter sw = new StreamWriter(ms);
		FormatFromBlocks(rootBlock, sw, 0, 0);
		sw.Flush();
		ms.Position = 0;
		byte [] stuff = new byte[ms.Length];
		ms.Read(stuff, 0, (int)ms.Length);
		Debug.Log("" + ms.Length);
        return new string(stuff.Select(a => (char)a).ToArray());
	}

	private static void FormatFromBlocks(JsBlock rootBlock, StreamWriter sw, int indent, int prevtextlen) {
		int thisLen = ComputeBlockLength(rootBlock);
		bool linebreak = thisLen > 80;
		if (indent > 0 && prevtextlen + thisLen > 80) {
			sw.Write('\n');
			for(int i = 0; i < indent; i++) {
				sw.Write("    ");
			}
		}
		for(int i = 0; i < rootBlock.texts.Length; i++) {
			var sects = SplitNoStrings(rootBlock.texts[i], ',');
			bool f = true;
			foreach(var sect in sects) {
				if (!f) {
					sw.Write(',');
					if (linebreak) {
						sw.Write('\n');
						for(int j = 0; j < indent; j++) {
							sw.Write("    ");
	                    }
					} else {
						sw.Write(' ');
					}
                }
                sw.Write(StripHeadingWhitespace(sect));
				f = false;
			}
			if (i < rootBlock.childBlocks.Length) {
				FormatFromBlocks(rootBlock.childBlocks[i], sw, indent + 1, rootBlock.texts[i].Length);
				if (rootBlock.texts[i].Length + ComputeBlockLength(rootBlock.childBlocks[i]) > 80) {
					sw.Write('\n');
					for(int j = 0; j < indent; j++) {
						sw.Write("    ");
	                }
				}
            }
		}
	}

	private static string StripHeadingWhitespace(string txt) {
		int i = 0;
		for (; i < txt.Length && char.IsWhiteSpace(txt[i]); i++) {
		}
		return txt.Substring(i);
	}

	private static JsBlock ParseBlocks(string input) {
		MemoryStream ms = new MemoryStream();
		StreamWriter writer = new StreamWriter(ms);
		writer.Write(input);
		writer.Flush();
		ms.Position = 0;
		return ParseBlocks(ms, '\0');
	}

	private static JsBlock ParseBlocks(MemoryStream input, char terminator = '\0') {
		int i = 0;
		List<JsBlock> children = new List<JsBlock>();
		List<string> texts = new List<string>();
		string currtxt = "";
		bool inText = false;
		char textDelimiter = '\0';
		bool breakChar = false;
		while(input.Position < input.Length) {
			var c = (char)input.ReadByte();
			if (!inText && (c == '\"' || c == '\'')) {
				inText = true; 
				textDelimiter = c;
			} else if (inText && !breakChar && c == textDelimiter) {
				inText = false;
			}

			if (c != terminator || inText) {
                currtxt += c;
			}

            if (c == '{' && !inText) {
				children.Add(ParseBlocks(input, '}'));
				texts.Add(currtxt);
				currtxt = "}";
            } else if (c == '[' && !inText) {
				children.Add(ParseBlocks(input, ']'));
				texts.Add(currtxt);
				currtxt = "]";
			}

			breakChar = false;
			if (c == '\\') {
				breakChar = true;
			}
			if (c == terminator && !inText) {
				break;
			}
		}
		texts.Add(currtxt);
		return new JsBlock {
			texts = texts.ToArray(),
			childBlocks = children.ToArray()
		};
	}

	private static int ComputeBlockLength(JsBlock block) {
		int ret = 0;
		foreach(var txt in block.texts) {
			ret += txt.Length;
		}
		foreach(var cb in block.childBlocks) {
			ret += ComputeBlockLength(cb);
		}
		return ret;
	}


	private static string AddIndent(string input) {
		string output = "";
		bool inString = false;
		bool isEscape = false;
		bool added = false;
		int level = 0;
		for (int i = 0; i < input.Length; i++) {
			char prev = i > 0 ? input[i-1] : '\0';
			char next = i < input.Length-1 ? input[i+1] : '\0';
			char c = input[i];
			if (IsStringDelimiter(c) && !isEscape) {
				inString = !inString;
			}
			if (c == '{' && !inString) {
				level++;
			}
			if (next == '}' && !inString) {
				level--;
			}
			output += c;
			if (c == '\n') {
				for (int j = 0; j < level * TAB_WIDTH; j++) {
					output += ' ';
				}
			}
			isEscape = false;
			if (c == '\\') {
				isEscape = true;
			}
		}
		return output;
	}


	private static string AddWhiteSpaces(string input) {
		string output = "";
		bool inString = false;
		bool isEscape = false;
		bool added = false;
		for (int i = 0; i < input.Length; i++) {
			char prev = i > 0 ? input[i-1] : '\0';
			char next = i < input.Length-1 ? input[i+1] : '\0';
			char c = input[i];
			if (IsStringDelimiter(c) && !isEscape) {
				inString = !inString;
			}
			if ((c == '{' || c == '=') && !added && !inString) {
				//output += ' ';
			}
			added = false;
			output += c;
			if (((c == '=' && input[i+1] != '=') || c == ':' || c == ';') && !inString) {
				//output += ' ';
				added = true;
			}
			isEscape = false;
			if (c == '\\') {
				isEscape = true;
			}
		}
		return output;
	}


	private static string AddBracketNewlines(string input) {
		string output = "";
		bool inString = false;
		bool isEscape = false;
		for (int i = 0; i < input.Length; i++) {
			char prev = i > 0 ? input[i-1] : '\0';
			char next = i < input.Length-1 ? input[i+1] : '\0';
			char c = input[i];
			if (IsStringDelimiter(c) && !isEscape) {
				inString = !inString;
			}
			if (c == '}' && !inString) {
				output += '\n';
			}
			output += c;
			if ((c == '{' || c == ',' ) && !inString) {
				output += '\n';
			}
			if (c == ';' && !inString) {
				output += '\n';
			}
			isEscape = false;
			if (c == '\\') {
				isEscape = true;
			}
		}
		return output;
	}

	private static string RemoveWhitespaces(string input) {
		string output = "";
		bool inString = false;
		bool isEscape = false;
		//foreach(char c in input) {
		for (int i = 0; i < input.Length; i++) {
			char prev = i > 0 ? input[i-1] : '\0';
			char next = i < input.Length-1 ? input[i+1] : '\0';
			char c = input[i];
			if (IsStringDelimiter(c) && !isEscape) {
				inString = !inString;
			}
			if (inString || !char.IsWhiteSpace(c) || (char.IsLetterOrDigit(prev) || char.IsLetterOrDigit(next))) {
				output += c;
			}
			isEscape = false;
			if (c == '\\') {
				isEscape = true;
			}
		}
		return output;
	}

	private static string [] SplitNoStrings(string str, char delimiter) {
		List<string> strings = new List<string> ();
		int N = 0;
		string curr = "";
		bool inString = false;
		while (N < str.Length) {
			char c = str[N];
			if (IsStringDelimiter(c)) {
				inString = !inString;
			}
			if (c == delimiter && !inString) {
				strings.Add(curr);
				curr = "";
			} else {
				curr += c;
			}
			N++;
		}
		strings.Add(curr);
		return strings.ToArray();
	}	

	private static bool IsStringDelimiter(char c) {
		return c == '\"' || c == '\'';
	}


}
