import optparse
import keras
import keras.backend
import neuon.details.primitives
import tensorflow
import os


def freeze_session(model, keep_var_names=None):
    session = keras.backend.get_session()
    graph = session.graph
    with graph.as_default():
        freeze_var_names = list(set(v.op.name for v in tensorflow.global_variables()).difference(keep_var_names or []))
        output_names = [out.op.name for out in model.outputs]
        output_names += [v.op.name for v in tensorflow.global_variables()]
        input_graph_def = graph.as_graph_def()
        for node in input_graph_def.node:
            node.device = ""

        frozen_graph = tensorflow.graph_util.convert_variables_to_constants( session, input_graph_def, output_names, freeze_var_names)
        return frozen_graph


def main():
    os.environ["CUDA_VISIBLE_DEVICES"] = "0"
    parser = optparse.OptionParser()
    parser.add_option("-k", "--keras-model", dest="kmodel", help="model", metavar="FILE")
    parser.add_option("-t", "--tensorflow-model", dest="tmodel", help="model", metavar="FILE")

    (options, args) = parser.parse_args()

    model = keras.models.load_model(options.kmodel, custom_objects={'contrastive_loss': neuon.details.primitives.contrastive_loss, 'keras': keras})
    model.summary()

    frozen_graph = freeze_session(model, options.tmodel)
    tensorflow.train.write_graph(frozen_graph, os.path.dirname(options.tmodel), os.path.basename(options.tmodel), as_text=False)


if __name__ == '__main__':
    main()
