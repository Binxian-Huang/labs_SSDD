from spyne import Application, rpc, ServiceBase, Unicode
from spyne.protocol.soap import Soap11
from spyne.server.wsgi import WsgiApplication

class ConvertTextService(ServiceBase):
    @rpc(Unicode, _returns=Unicode)
    def convert_text(ctx, text):
        converted_text = ' '.join(text.split())
        return converted_text
    
application = Application([ConvertTextService],
                        tns='http://myservice.com/convert-text',
                        in_protocol=Soap11(validator='lxml'),
                        out_protocol=Soap11())

wsgi_application = WsgiApplication(application)

if __name__ == '__main__':
    import logging
    from wsgiref.simple_server import make_server
    logging.basicConfig(level=logging.DEBUG)
    logging.getLogger('spyne.protocol.xml').setLevel(logging.DEBUG)
    logging.info("listening to http://0.0.0.0:8000")
    logging.info("wsdl is at: http://localhost:8000/convert_text?wsdl")
    server = make_server('0.0.0.0', 8000, wsgi_application)
    server.serve_forever()
