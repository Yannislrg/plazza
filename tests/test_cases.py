"""
Test cases data for Plazza functional tests.
"""

TEST_CONFIG = (0.05, 2, 1000)
STATUS_CONFIG = (1.0, 2, 1000)

TEST_DATA = [
    {
        "name": "Classic Case",
        "command": "margarita S x1",
        "expected": ["[Ready] Margarita", "Order #1 completely ready!"]
    },
    {
        "name": "No Spaces Case",
        "command": "reginaXXLx1",
        "expected": ["[Ready] Regina"]
    },
    {
        "name": "Status Basic",
        "config": STATUS_CONFIG,
        "steps": [
            {"send": "status", "expect": ["No active kitchen."]},
            {"send": "margarita S x1", "expect": []},
            {"send": "status", "expect": ["cooking Margarita", "dough: 4"]}
        ]
    },
    {
        "name": "Kitchen Timeout (5s)",
        "config": (0.1, 1, 1000),
        "steps": [
            {"send": "margarita S x1", "expect": ["[Ready] Margarita"]},
            {"send": "status", "expect": ["Kitchen #0"]},
            {"sleep": 7, "expect": []},
            {"send": "status", "expect": ["No active kitchen."]}
        ]
    },
    {
        "name": "Load Balancing - Saturation",
        "config": (5.0, 1, 1000), # 1 cook -> capacity 2
        "steps": [
            {"send": "margarita S x3", "expect": []}, # 2 in K0, 1 in K1
            {"send": "status", "expect": ["Kitchen #0", "Kitchen #1"]},
            {"send": "status", "expect": ["Kitchen #0 [2/2]", "Kitchen #1 [1/2]"]}
        ]
    },
    {
        "name": "Ingredient Regeneration",
        "config": (2.0, 1, 500),
        "steps": [
            {"send": "margarita S x1", "expect": []},
            {"send": "status", "expect": ["dough: 4"]},
            {"sleep": 1.5, "expect": []},
            {"send": "status", "expect": ["dough: "]}
        ]
    },
    {
        "name": "Full Recipe Ingredients",
        "config": (5.0, 1, 1000),
        "steps": [
            {"send": "regina S x1", "expect": []},
            {"send": "status", "expect": [
                "dough: 4", "tomato: 4", "gruyere: 4", "ham: 4", "mushrooms: 4"
            ]}
        ]
    },
    {
        "name": "Complex Multi-Order Dispatch",
        "config": (0.01, 5, 1000),
        "command": "margarita S x2; regina M x2; americana L x2; fantasia XXL x2",
        "expected": [
            "[Ready] Margarita", "[Ready] Regina",
            "[Ready] Americana", "[Ready] Fantasia",
            "Order #1 completely ready!"
        ]
    }
]
