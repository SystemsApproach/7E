from docutils import nodes
from sphinx.util.docutils import SphinxDirective


class takeaway_node(nodes.General, nodes.Element):
    pass


class TakeawayDirective(SphinxDirective):
    has_content = True

    def run(self):
        node = takeaway_node()
        self.state.nested_parse(self.content, self.content_offset, node)
        return [node]


def visit_takeaway_html(self, node):
    self.body.append('<div class="takeaway">\n')


def depart_takeaway_html(self, node):
    self.body.append('</div>\n')


def visit_takeaway_latex(self, node):
    self.body.append('\n\\begin{takeawaybox}\n')


def depart_takeaway_latex(self, node):
    self.body.append('\n\\end{takeawaybox}\n')


def visit_takeaway_text(self, node):
    self.new_state(0)


def depart_takeaway_text(self, node):
    self.end_state()


def setup(app):
    app.add_node(
        takeaway_node,
        html=(visit_takeaway_html, depart_takeaway_html),
        latex=(visit_takeaway_latex, depart_takeaway_latex),
        text=(visit_takeaway_text, depart_takeaway_text),
    )
    app.add_directive('takeaway', TakeawayDirective)
    return {'version': '0.1', 'parallel_read_safe': True}
