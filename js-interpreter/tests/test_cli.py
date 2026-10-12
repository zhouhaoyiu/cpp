"""Exercise the real built commands; timeouts also catch lexer/parser loops."""
import pathlib
import subprocess
import sys
import tempfile
import unittest

NODE, LEXER, PARSER = map(lambda p: str(pathlib.Path(p).resolve()), sys.argv[1:4])
sys.argv[1:] = []


def run(command, source=None, stdin=None):
    with tempfile.TemporaryDirectory() as directory:
        args = [command]
        if source is not None:
            path = pathlib.Path(directory) / 'input.js'
            path.write_text(source)
            args.append(str(path))
        return subprocess.run(args, input=stdin, text=True, capture_output=True, timeout=5)


class Commands(unittest.TestCase):
    def test_multiple_statements_execute(self):
        result = run(NODE, 'let a=1;\nlet b=2;\na=a+b;')
        self.assertEqual(result.returncode, 0, result.stderr)
        self.assertRegex(result.stdout, r'\| a\s*\| 3(?:\.0+)?\s*\|')
        self.assertRegex(result.stdout, r'\| b\s*\| 2\s*\|')

    def test_ast_command_prints_children_and_siblings(self):
        result = run(PARSER, 'let first=1;\nlet second=first+2;')
        self.assertEqual(result.returncode, 0, result.stderr)
        self.assertIn('token=second', result.stdout)
        self.assertIn('token=+', result.stdout)

    def test_empty_file(self):
        for command in (NODE, LEXER, PARSER):
            with self.subTest(command=command):
                result = run(command, '')
                self.assertEqual(result.returncode, 0, result.stderr)

    def test_invalid_character_exits_nonzero(self):
        for command in (NODE, LEXER, PARSER):
            with self.subTest(command=command):
                result = run(command, 'let a=@;')
                self.assertEqual(result.returncode, 1, result.stderr)
                self.assertIn('unexpected character', result.stderr)

    def test_unterminated_string_file(self):
        for source in ('let s="abc', 'let s="abc\n', 'let s="abc\\'):
            for command in (NODE, LEXER, PARSER):
                with self.subTest(command=command, source=source):
                    result = run(command, source)
                    self.assertEqual(result.returncode, 1, result.stderr)
                    self.assertIn('unterminated string', result.stderr)

    def test_valid_string_file(self):
        result = run(LEXER, 'let s="abc";')
        self.assertEqual(result.returncode, 0, result.stderr)
        self.assertIn('<STRING, "abc",', result.stdout)

    def test_interactive_errors_do_not_poison_next_input(self):
        result = run(NODE, stdin='let a=@;\nlet b=2;\n.show\n.exit\n')
        self.assertEqual(result.returncode, 0, result.stderr)
        self.assertIn('unexpected character', result.stderr)
        self.assertRegex(result.stdout, r'\| b\s*\| 2\s*\|')

    def test_interactive_eof(self):
        self.assertEqual(run(NODE, stdin='').returncode, 0)


if __name__ == '__main__':
    unittest.main(verbosity=2)
