import subprocess
import unittest

class QuadraticEquationE2ETest(unittest.TestCase):
    def run_program(self, input_args):
        result = subprocess.run(
            ["./out"] + input_args,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            text=True
        )
        return result.stdout.strip().split("\n"), result.stderr.strip()

# one-equation section
    def test_invalid_input_1(self):
        """No values ​​are passed."""
        input_args = []
        output, error = self.run_program(input_args)
        
        expected_outputs = [
            ""
        ]

        self.assertEqual(output, expected_outputs)

    def test_invalid_input_2(self):
        """One numeric value passed."""
        input_args = ["1"]
        output, error = self.run_program(input_args)
        
        expected_outputs = [
            "(1,,) => invalid input",
        ]
        
        self.assertEqual(output, expected_outputs)

    def test_invalid_input_3(self):
        """One negative numeric value passed (8 bytes)."""
        input_args = ["-1"]
        output, error = self.run_program(input_args)
        
        expected_outputs = [
            "(-1,,) => invalid input",
        ]
        
        self.assertEqual(output, expected_outputs)

    def test_invalid_input_4(self):
        """One edge numeric value passed (8 bytes)."""
        input_args = ["9223372036854775807"]
        output, error = self.run_program(input_args)
        
        expected_outputs = [
            "(9223372036854775807,,) => invalid input",
        ]
        
        self.assertEqual(output, expected_outputs)

    def test_invalid_input_5(self):
        """One edge negative numeric value passed (8 bytes)."""
        input_args = ["-9223372036854775808"]
        output, error = self.run_program(input_args)
        
        expected_outputs = [
            "(-9223372036854775808,,) => invalid input",
        ]
        
        self.assertEqual(output, expected_outputs)

    def test_invalid_input_6(self):
        """One negative out-of-range numeric value passed (8 bytes)."""
        input_args = ["9223372036854775808"]
        output, error = self.run_program(input_args)
        
        expected_outputs = [
            "(,,) => invalid input",
        ]
        
        self.assertEqual(output, expected_outputs)

    def test_invalid_input_7(self):
        """One negative out-of-range numeric value passed (8 bytes)."""
        input_args = ["-9223372036854775809"]
        output, error = self.run_program(input_args)
        
        expected_outputs = [
            "(,,) => invalid input",
        ]
        
        self.assertEqual(output, expected_outputs)

    def test_invalid_input_8(self):
        """One non-numeric value passed (8 bytes)."""
        input_args = ["qwerty"]
        output, error = self.run_program(input_args)
        
        expected_outputs = [
            "(,,) => invalid input",
        ]
        
        self.assertEqual(output, expected_outputs)

    def test_invalid_input_9(self):
        """Two numeric values passed."""
        input_args = ["1", "1"]
        output, error = self.run_program(input_args)
        
        expected_outputs = [
            "(1,1,) => invalid input",
        ]
        
        self.assertEqual(output, expected_outputs)

    def test_invalid_input_10(self):
        """Invalid and valid values passed."""
        input_args = ["qwerty", "1"]
        output, error = self.run_program(input_args)
        
        expected_outputs = [
            "(,1,) => invalid input",
        ]
        
        self.assertEqual(output, expected_outputs)

    def test_invalid_input_11(self):
        """Valid and invalid values passed."""
        input_args = ["1", "qwerty"]
        output, error = self.run_program(input_args)
        
        expected_outputs = [
            "(1,,) => invalid input",
        ]
        
        self.assertEqual(output, expected_outputs)

    def test_invalid_input_12(self):
        """Three values provided."""
        input_args = ["1", "1", "qwerty"]
        output, error = self.run_program(input_args)
        
        expected_outputs = [
            "(1,1,) => invalid input",
        ]
        
        self.assertEqual(output, expected_outputs)

    def test_invalid_input_13(self):
        """Three values provided."""
        input_args = ["qwerty", "1", "qwerty"]
        output, error = self.run_program(input_args)
        
        expected_outputs = [
            "(,1,) => invalid input",
        ]
        
        self.assertEqual(output, expected_outputs)

    def test_invalid_input_14(self):
        """Three values provided."""
        input_args = ["1", "qwerty", "qwerty"]
        output, error = self.run_program(input_args)
        
        expected_outputs = [
            "(1,,) => invalid input",
        ]
        
        self.assertEqual(output, expected_outputs)

    def test_invalid_input_15(self):
        """Three values provided."""
        input_args = ["qwerty", "qwerty", "1"]
        output, error = self.run_program(input_args)
        
        expected_outputs = [
            "(,,1) => invalid input",
        ]
        
        self.assertEqual(output, expected_outputs)

    def test_invalid_input_15(self):
        """Three values provided."""
        input_args = ["qwerty", "qwerty", "qwerty"]
        output, error = self.run_program(input_args)
        
        expected_outputs = [
            "(,,) => invalid input",
        ]
        
        self.assertEqual(output, expected_outputs)

    def test_invalid_input_16(self):
        """Zeros only."""
        input_args = ["0", "0", "0"]
        output, error = self.run_program(input_args)
        
        expected_outputs = [
            "(0,0,0) => invalid input",
        ]
        
        self.assertEqual(output, expected_outputs)

    def test_valid_input_1(self):
        """Two real roots."""
        input_args = ["1", "-2", "-3"]
        output, error = self.run_program(input_args)
        
        expected_outputs = [
            "(1,-2,-3) => (-1.000000, -1.000000) Xmin=3.000000",
        ]
        
        self.assertEqual(output, expected_outputs)

    def test_valid_input_2(self):
        """No roots."""
        input_args = ["1", "2", "3"]
        output, error = self.run_program(input_args)
        
        expected_outputs = [
            "(1,2,3) => (No root 1, No root 2) Xmin=1.000000",
        ]
        
        self.assertEqual(output, expected_outputs)

    def test_valid_input_4(self):
        """Same roots."""
        input_args = ["1", "-6", "9"]
        output, error = self.run_program(input_args)
        
        expected_outputs = [
            "(1,-6,9) => (3.000000, -3.000000) Xmin=3.000000",
        ]
        
        self.assertEqual(output, expected_outputs)

    def test_valid_input_3(self):
        """Two roots."""
        input_args = ["1", "0", "-2"]
        output, error = self.run_program(input_args)
        
        expected_outputs = [
            "(1,0,-2) => (-1.414214, 0.000000) Xmin=1.414214",
        ]
        
        self.assertEqual(output, expected_outputs)

    def test_valid_input_5(self):
        """Two roots."""
        input_args = ["0", "0", "1"]
        output, error = self.run_program(input_args)
        
        expected_outputs = [
            "(0,0,1) => invalid input",
        ]
        
        self.assertEqual(output, expected_outputs)

    def test_valid_input_6(self):
        """No Xmin."""
        input_args = ["0", "1", "1"]
        output, error = self.run_program(input_args)
        
        expected_outputs = [
            "(0,1,1) => (-1.000000, No root 2) No Xmin",
        ]
        
        self.assertEqual(output, expected_outputs)

# multiple-equation section
    def test_m_input_1(self):
        """Three invalid equations."""
        input_args = ["qwerty", "-2", "-3", "qwerty", "qwerty", "qwerty", "1", "qwerty", "-3"]
        output, error = self.run_program(input_args)
        
        expected_outputs = {
            "(1,,-3) => invalid input",
            "(,-2,-3) => invalid input",
            "(,,) => invalid input",
        }
        
        self.assertEqual(set(output), expected_outputs)

    def test_m_input_2(self):
        """Two of three are invalid equations."""
        input_args = ["1", "qwerty", "-3", "1", "-2", "-3", "1", "qwerty", "-3"]
        output, error = self.run_program(input_args)
        
        expected_outputs = {
            "(1,,-3) => invalid input",
            "(1,,-3) => invalid input",
            "(1,-2,-3) => (-1.000000, -1.000000) Xmin=3.000000",
        }
        
        self.assertEqual(set(output), expected_outputs)

    def test_m_input_3(self):
        """One of three is an invalid equation."""
        input_args = ["1", "qwerty", "-3", "1", "-2", "-3", "1", "-2", "-3"]
        output, error = self.run_program(input_args)
        
        expected_outputs = {
            "(1,-2,-3) => (-1.000000, -1.000000) Xmin=3.000000",
            "(1,,-3) => invalid input",
            "(1,-2,-3) => (-1.000000, -1.000000) Xmin=3.000000",
        }
        
        self.assertEqual(set(output), expected_outputs)

    def test_m_input_4(self):
        """Three valid equations."""
        input_args = ["1", "-2", "-3", "1", "-2", "-3", "1", "-2", "-3"]
        output, error = self.run_program(input_args)
        
        expected_outputs = {
            "(1,-2,-3) => (-1.000000, -1.000000) Xmin=3.000000",
            "(1,-2,-3) => (-1.000000, -1.000000) Xmin=3.000000",
            "(1,-2,-3) => (-1.000000, -1.000000) Xmin=3.000000",
        }
        
        self.assertEqual(set(output), expected_outputs)

    def test_m_input_5(self):
        """Number of outputs."""
        input_args = [str(i) for i in range(0, 100000)]
        output, error = self.run_program(input_args)
        
        expected_outputs = 33334
        
        self.assertEqual(len(output), expected_outputs)

if __name__ == "__main__":
    unittest.main()
