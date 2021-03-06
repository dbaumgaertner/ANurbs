import unittest
import anurbs as an
from numpy.testing import assert_array_almost_equal, assert_almost_equal
import os

class TestPointOnSurfaceProjection(unittest.TestCase):
    def test_point_inversion(self):
        model = an.Model()
        model.load(f'{os.path.dirname(__file__)}/data/point_on_surface_projection.ibra')

        surface = model.of_type('NurbsSurfaceGeometry3D')[0].data

        projection = an.PointOnSurfaceProjection3D(surface)

        test_data = [
            [0.2486685842, 0.1107439772],
            [0.9978910512, 0.875821514],
            [0.0830195928, 0.9259765381],
            [0.9496291005, 0.1502956968],
            [0.459878301, 0.6225033205],
            [0.6025333179, 0.9861121243],
            [0.0790516166, 0.5709693747],
            [0.5655969403, 0.2574736384],
            [0.8305452102, 0.5624009979],
            [0.2794794688, 0.8097236998],
            [0.3071577155, 0.3822328865],
            [0.99964868, 0.4057108985],
            [0.0189992101, 0.2765154416],
            [0.0056710457, 0.0347536937],
            [0.7897584256, 0.8313703019],
            [0.7195281758, 0.1178902761],
            [0.4422092407, 0.0883118692],
            [0.6658555305, 0.4627695477],
            [0.5076745905, 0.8196537363],
            [0.2832733436, 0.6378145738],
            [0.0645325822, 0.7572020105],
            [0.9879030846, 0.6847038878],
            [0.7960193063, 0.3109410034],
            [0.6945010543, 0.6643853852],
            [0.8336644866, 0.0037464825],
            [0.3930552445, 0.9678212548],
            [0.4785460939, 0.3808379352],
            [0.585713005, 0.0070576593],
            [0.3991026969, 0.2632817497],
            [0.2006969681, 0.2421369582],
            [0.1202849104, 0.4221172297],
            [0.7358854449, 0.9698849781],
            [0.5188882311, 0.5018097253],
            [0.3654126839, 0.5201963845],
            [0.6403577298, 0.8467138102],
            [0.8726223264, 0.9882953218],
            [0.0928173764, 0.1633056552],
            [0.155525548, 0.028770891],
            [0.0010793009, 0.456353415],
            [0.5686607461, 0.7125619164],
            [0.1982382905, 0.7190583882],
            [0.3281335357, 0.0049134241],
            [0.8250263221, 0.6903648873],
            [0.2025081111, 0.9819264952],
            [0.2204824454, 0.5096595127],
            [0.3834477823, 0.8546278755],
            [0.9842980774, 0.0390799376],
            [0.1078386875, 0.3153865423],
            [0.3720683969, 0.7303910846],
            [0.9586810097, 0.5417624263],
            [0.6905896192, 0.2880027598],
            [0.5518418474, 0.1284805062],
            [0.7797412704, 0.4522544888],
            [0.9001546897, 0.7999264513],
            [0.7019192277, 0.5565699151],
            [0.8618938363, 0.8969601173],
            [0.573701815, 0.3725593828],
            [0.5645628416, 0.6001849126],
            [0.9691815544, 0.2581378097],
            [0.2934454783, 0.2084521582],
            [0.5663407671, 0.890895538],
            [0.8255418832, 0.1905749553],
            [0.9205142534, 0.4522520338],
            [0.4010180167, 0.4240173988],
            [0.6520043102, 0.7641253019],
            [0.4829338433, 0.2314186642],
            [0.6469415439, 0.1985799857],
            [0.0100815608, 0.8321075117],
            [0.2825933291, 0.9272068771],
            [0.2162422287, 0.4111952472],
            [0.1691770848, 0.6298445815],
            [0.9101820793, 0.3544982711],
            [0.7672993754, 0.7465931879],
            [0.9772622413, 0.7692543193],
            [0.0181493168, 0.3759801157],
            [0.7127572883, 0.0072015384],
            [0.5138806088, 0.9528929917],
            [0.8881267998, 0.633660035],
            [0.1209032033, 0.8468105364],
            [0.3423240503, 0.1053561797],
            [0.0060057966, 0.1696525231],
            [0.8454230683, 0.0981951589],
            [0.1910913443, 0.8742888891],
            [0.9726746664, 0.9913883214],
            [0.714155909, 0.3804675184],
            [0.8380541009, 0.3771821253],
            [0.4770630204, 0.6988195776],
            [0.1394817876, 0.4965714307],
            [0.2955455749, 0.7173407975],
            [0.3722430087, 0.3385346254],
            [0.8603301383, 0.259464834],
            [0.0317961453, 0.6580847188],
            [0.0028120549, 0.9568088497],
            [0.4066441252, 0.1674200367],
            [0.6186872817, 0.3124124684],
            [0.001451293, 0.5436467522],
            [0.6059298895, 0.5078425922],
            [0.6547791262, 0.0598361939],
            [0.1788401213, 0.1605233313],
            [0.460314065, 0.8863533544],
        ]

        for expected_uv in test_data:
            expected_point = surface.point_at(*expected_uv)

            success, u, v, point = projection.get(expected_point)

            self.assertTrue(success)
            assert_almost_equal([u, v], expected_uv)
            assert_almost_equal(point, expected_point)


if __name__ == '__main__':
    unittest.main()
